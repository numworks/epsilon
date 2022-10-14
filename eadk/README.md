# EADK

EADK is the software development kit to build independent, external apps for Epsilon. With EADK, you'll be able to build NWA files that you will then be able to [load on your NumWorks calculator](https://my.numworks.com/apps).

## Functions you may use

All the functions you can use are in `eadk.h`. They should hopefully all be pretty self-explanatory.

## About NWA files

An NWA file is really just an ELF file that meets certain requirements.

 - A `main` symbol that will be the entry point of the app.
 - A `.rodata.eadk_api_level` section containing exactly one 32 bit value.
 - A `.rodata.eadk_app_name` section containing a NULL-terminated NFKD UTF-8 string (name of the app).
 - A `.rodata.eadk_app_icon` section containing an NWI-encoded 55x56 image (icon of the app).

Optionally, this ELF file can reference a `eadk_external_data` symbol that will be resolved when installing the app on the calculator.

## Some examples

Don't hesitate to browse our sample apps:
 - In [pure C](https://github.com/numworks/epsilon-sample-app), which is probably the easiest to start with
 - In [C++](https://github.com/numworks/epsilon-sample-app-cpp)
 - In [Rust](https://github.com/numworks/epsilon-sample-app-cpp)
