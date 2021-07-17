# gimp_quick_guides
Handy GIMP plugin to add useful image guides with a single click.

# Installing
## Install required libraries and headers
Assuming that you already have a functional C compiler and development packages
installed on your system, run the following command to install GIMP development
packages. This will allow you to compile this C-API-based GIMP plugin.
```
sudo apt-get install libgimp2.0-dev
```

## `./configure && make`
Run the classic `./configure && make` combo. Add `sudo make install`, as shown below for
installation to appropriate directories for your GIMP application installation to pick up.
**NOTE:** For plugins that are written using the C APIs and compile to loadable shared
objects, GIMP will need to be restarted for any changes in those plugins to be detected.
```
./configure && make && sudo make install
```

## How do I know if it's working?
Simple:
 1. Start up your copy of GIMP
 2. Either create a new image or open an existing image
 3. Navigate to the following application menu: `Image` -> `Guides` and you should see `Quick Guides...`.
 4. Clicking on `Quick Guides...` should add several image guides. This is success!

