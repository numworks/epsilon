# External applications

## Quick installation

1. Plug your calculator to your computer, 'THE CALCULATOR IS CONNECTED' sreen should be displayed.
2. To compile and flash the external application, run the following command:
```shell
make clean
make
```

By default, the application is sent to the calculator as the first external application. You might set the application index as in the following example:
```shell
make clean
make APPLICATION_INDEX=1
```

## Developing one's own app

The *utils* folder contains the requirements expected by Epsilon for your application to run.

### App info

The binary should be prefixed with a header `AppInfo` that gathers the following information:
- A header set to 0xBABEC0DE
- The API level that should describes the version kernel API required
- The offset in bytes to where the title of the application is stored in the binary
- The size of the compressed icon in bytes
- The offset in bytes to where the compressed icon is stored in the binary
- The offset in bytes to the entry point of the application
- The size in bytes of the application
- A footer set to 0xBABEC0DE

The files utils/app_info.h utils/app_info.cpp are meant to be used to generate this header.

The icon is expected to be compressed with a LZ4 high compression. The python script utils/inliner.pyis meant to be used to inline compressed icon in C code from a PNG image with the command
```shell
python3 utils/inliner.py --png icon.png --header icon.h --cimplementation icon.cpp
```

### System calls

The available systems calls interface is given in utils/svc.h.

### Memory layouts

The external flash is divided in two equal-sized slot where can live each instance of the software. The external apps are layouted at the end of each slot: 11x64kB are assigned to each external apps section. We flash the external application in each slot (at 0x90350000 and 0x90750000) to avoid requiring the running slot information.
