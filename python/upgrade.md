# Steps to upgrade MicroPython

- Clone the MicroPython project and checkout the current version
- Find the current patches and save them or make sure they were integrated to the next MicroPython version
        git diff Path/to/Upsilon/python/src/py Path/to/MicroPython/py
- Checkout the new version in the MicroPython project
- Copy the MicroPython py files in Upsilon python/src/py folder
- Update Upsilon/python/Makefile, Upsilon/python/port/genhdr/qstrdefs.in.h and Upsilon/python/port/genhdr/moduledefs.h following the instructions in the files
- Update other Upsilon/python/port/genhdr/ files :
        Get a clean copy of MicroPython
        Copy our mpconfigport.h over the "bare-arm" port of MicroPython
        "make" the bare-arm port of MicroPython (don't worry if it doesn't finish)
        Copy the wanted build/genhdr files to Upsilon/python/port/genhdr/
- Put back the patches from the first step if needed
