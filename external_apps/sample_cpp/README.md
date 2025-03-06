# Sample C++ app for Epsilon

<img src="/doc/screenshots.gif?raw=true" alt="Sample C++ app for the NumWorks graphing calculator" width="300" align="right">

This is a sample C++ app.

## Setup

To build this app on a simulator, you'll just need a C compiler (`gcc` is expected on Windows and Linux and `clang` is expected on MacOS).

```shell
./setup.sh
```

### Prepare web and native simulators

To run the apps on web or native simulators, an `epsilon_simulators` folder is expected in the same root as this folder by default.

It should contain the target epsilon simulator, see [instructions to build it](../README.md).

Simulator path can also be overridden with the `SIMULATOR` compilation flag :
```shell
make PLATFORM=web SIMULATOR=epsilon.html run
make PLATFORM=simulator HOST=linux SIMULATOR=epsilon.bin run
make PLATFORM=simulator HOST=macos SIMULATOR=epsilon.app/Contents/MacOS/Epsilon run
make PLATFORM=simulator HOST=windows SIMULATOR=epsilon.exe run
```

## Run the app

Launch the app on the platform of your choice.

### On a device

Plug in a calculator, without exiting the `CALCULATOR IS CONNECTED` screen, run
```shell
make PLATFORM=device run
```

You should now have a `output/device/voord.nwa` file that you can distribute! Anyone can now install it on their calculator from the [NumWorks online uploader](https://my.numworks.com/apps).

### On a web simulator

On a separate shell, run
```shell
make server
```

In the other shell, run
```shell
source "./emsdk/emsdk_env.sh"
make PLATFORM=web run
```

A navigator should open on a web simulator of your app.

### On native simulator (MacOS, Linux, Windows)

```shell
make PLATFORM=simulator run
```

You can also debug your app using
```shell
make PLATFORM=simulator debug
```
