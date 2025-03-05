# RPN app for Epsilon written in C

This is a basic RPN app written in C to use on a [NumWorks calculator](https://www.numworks.com).

## Run the app

To build this app on a simulator, you'll just need a C compiler (`gcc` is expected on Windows and Linux and `clang` is expected on MacOS).

To build it for a NumWorks device, you'll additionally need [Node.js](https://nodejs.org/en/) ([Installation with package manager](https://nodejs.org/en/download/package-manager/)). The C SDK for Epsilon apps is shipped as an npm module called [nwlink](https://www.npmjs.com/package/nwlink) that will automatically be installed at compile time.

```shell
make clean && make run
make debug
```

This should launch a simulator running your application (or a debugger targeting your application).

## License

This sample app is distributed under the terms of the BSD License. See LICENSE for details.

## Trademarks

NumWorks is a registered trademark.
