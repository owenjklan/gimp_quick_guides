/* GIMP Plug-in Template
 * Copyright (C) 2000  Michael Natterer <mitch@gimp.org> (the "Author").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Author of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Author.
 */
#include "config.h"

#include <string.h>

#include <glib.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"
#include "interface.h"
#include "render.h"

#include "plugin-intl.h"

/*  Constants  */
#define PROCEDURE_NAME   "gimp_quick_guides"

#define DATA_KEY_VALS    "plug_in_quick_guides"
#define DATA_KEY_UI_VALS "plug_in_quick_guides_ui"

#define PARASITE_KEY     "plug-in-quick-guides-options"

/*  Local function prototypes  */
static void   query (void);
static void   run   (const gchar      *name,
                     gint              nparams,
                     const GimpParam  *param,
                     gint             *nreturn_vals,
                     GimpParam       **return_vals);

/*  Local variables  */
const PlugInVals default_vals =
{
    0,
    1,
    2,
    0,
    FALSE
};

const PlugInImageVals default_image_vals =
{
    0
};

const PlugInDrawableVals default_drawable_vals =
{
  0
};

const PlugInUIVals default_ui_vals =
{
  TRUE
};

static PlugInVals         vals;
static PlugInImageVals    image_vals;
static PlugInDrawableVals drawable_vals;
static PlugInUIVals       ui_vals;


GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,  /* init_proc  */
  NULL,  /* quit_proc  */
  query, /* query_proc */
  run,   /* run_proc   */
};

MAIN ()

static void
query (void)
{
    gchar *help_path;
    gchar *help_uri;

    static GimpParamDef args[] =
    {
        { GIMP_PDB_INT32,    "run_mode",   "Interactive, non-interactive"    },
        { GIMP_PDB_IMAGE,    "image",      "Input image"                     },
        { GIMP_PDB_DRAWABLE, "drawable",   "Input drawable"                  },
        { GIMP_PDB_INT32,    "dummy",      "dummy1"                          },
        { GIMP_PDB_INT32,    "dummy",      "dummy2"                          },
        { GIMP_PDB_INT32,    "dummy",      "dummy3"                          },
        { GIMP_PDB_INT32,    "seed",       "Seed value (used only if randomize is FALSE)" },
        { GIMP_PDB_INT32,    "randomize",  "Use a random seed (TRUE, FALSE)" }
    };

    gimp_plugin_domain_register (PLUGIN_NAME, LOCALEDIR);

    help_path = g_build_filename (DATADIR, "help", NULL);
    help_uri = g_filename_to_uri (help_path, NULL, NULL);
    g_free (help_path);

    gimp_plugin_help_register ("http://developer.gimp.org/plug-in-template/help",
                             help_uri);

    gimp_install_procedure (
        PROCEDURE_NAME,
        "Quick Guides",
        "Help",
        "Owen Klan",
        "Owen Klan",
        "2021",
        N_("Quick Guides..."),
        "RGB*, GRAY*, INDEXED*",
        GIMP_PLUGIN,
        G_N_ELEMENTS (args), 0,
        args, NULL);

    gimp_plugin_menu_register (PROCEDURE_NAME, "<Image>/Image/Guides/");
}

#define SET_MAX_LENGTH 20
typedef struct __GuidesSet {
    gint32 length;
    gfloat set[SET_MAX_LENGTH];
} GuidesSet;

// Horizontal guides are expressed based on y position
GuidesSet thirds = {
    .length = 2,
    .set = {0.33, 0.66}
};
GuidesSet quarters = {
    .length = 3,
    .set = {0.25, 0.5, 0.75}
};
GuidesSet boundaries = {
    .length = 2,
    .set = {0.00, 1.00}
};

static void add_guideset_to_image(gint32 image_ID, GuidesSet *current_set)
{
    gint32 width;
    gint32 height;
    gint32 set_cursor;

    width = gimp_image_width(image_ID);
    height = gimp_image_height(image_ID);

    for (set_cursor = 0; set_cursor < current_set->length; set_cursor++) {
        gint32 y_pos = (gfloat)height * current_set->set[set_cursor];
        gint32 x_pos = (gfloat)width * current_set->set[set_cursor];

        gimp_image_add_hguide(image_ID, y_pos);
        gimp_image_add_vguide(image_ID, x_pos);
    }
}

static void
run (const gchar *name, gint n_params, const GimpParam *param,
     gint *nreturn_vals, GimpParam **return_vals)
{
    static GimpParam   values[1];
    GimpDrawable      *drawable;
    gint32             image_ID;
    GimpRunMode        run_mode;
    GimpPDBStatusType  status = GIMP_PDB_SUCCESS;

    *nreturn_vals = 1;
    *return_vals  = values;

    /*  Initialize i18n support  */
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
    textdomain (GETTEXT_PACKAGE);

    run_mode = param[0].data.d_int32;
    image_ID = param[1].data.d_int32;
    drawable = gimp_drawable_get (param[2].data.d_drawable);

    /*  Initialize with default values  */
    vals          = default_vals;
    image_vals    = default_image_vals;
    drawable_vals = default_drawable_vals;
    ui_vals       = default_ui_vals;

    if (strcmp (name, PROCEDURE_NAME) == 0) {
        if (run_mode != GIMP_RUN_NONINTERACTIVE) {
            add_guideset_to_image(image_ID, &thirds);
            add_guideset_to_image(image_ID, &quarters);
            add_guideset_to_image(image_ID, &boundaries);
        }
    } else {
        status = GIMP_PDB_CALLING_ERROR;
    }

    if (status == GIMP_PDB_SUCCESS) {
        if (run_mode != GIMP_RUN_NONINTERACTIVE)
            gimp_displays_flush ();

        gimp_drawable_detach (drawable);
    }

    values[0].type = GIMP_PDB_STATUS;
    values[0].data.d_status = status;
}
