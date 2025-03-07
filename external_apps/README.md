# External apps

This folder contains apps to run on a [NumWorks calculator](https://www.numworks.com).

- [rpn_cpp](rpn_cpp) : a RPN engine to be completed
- [sample_c](sample_c) : a C application plotting squares and using external data
- [sample_cpp](sample_cpp) : a space invaders game
- [sample_rust](sample_rust) : not C/C++ based, following setup instructions do not apply, see [sample_rust instructions](sample_rust/README.md).

## On device

Here are the instructions to run the app [rpn_cpp](rpn_cpp) on a Numworks calculator.

You can swap `rpn_cpp` for any other  C/C++ based external app.

### Setup nwlink

Before your first run, you'll need to install nwlink on your computer.
```shell
npm install -g nwlink
```

### Run the app

This will build and load your app on a plugged in calculator.

The calculator ust be on the `CALCULATOR IS CONNECTED` screen.

```shell
make -C rpn_cpp PLATFORM=device run
```

A `THIRD PART APPLICATIONS` message will appear, and the app should be accessible at the end of the Home menu.

### Distribute your .nwa app

After a run or a build
```shell
make -C rpn_cpp PLATFORM=device build
```

You should have a `rpn_cpp/output/device/rpn.nwa` file that you can distribute!

Anyone can now install it on their calculator from the [NumWorks online uploader](https://my.numworks.com/apps).

## Try your app on an Epsilon simulator



### Setup

You'll just need a C compiler (`gcc` is expected on Windows and Linux and `clang` is expected on MacOS).

Depending on you file system :
- Windows :
```shell
./setup.ps1
```
- Others :
```shell
chmod +x setup.sh & ./setup.sh
```

#### Prepare web and native simulators

By default, an [epsilon_simulators](epsilon_simulators/) folder is expected.

It contains platform-specific Epsilon simulators to run any external app on.
See instructions to build them from [Epsilon's root](../) :
- [Web](epsilon_simulators/web/README.md) : `epsilon.html`
- [Linux](epsilon_simulators/linux/README.md) : `epsilon.bin`
- [MacOS](epsilon_simulators/macos/README.md) : `epsilon.app/Contents/MacOS/Epsilon`
- [Windows](epsilon_simulators/windows/README.md) : `epsilon.exe`

Simulator path can also be overridden with the `SIMULATOR=[PATH_TO_SIMULATOR]` compilation flag.

#### Web simulator

On a separate shell, run
```shell
make -C rpn_cpp server
```

In the other shell, run :
- Windows
```shell
emsdk/emsdk_env.ps1
```
- Others
```shell
source emsdk/emsdk_env.sh
```

Finally, run

```shell
make -C rpn_cpp PLATFORM=web run
```

Your browser should display the simulator with the app inside.

### On native simulator (MacOS, Linux, Windows)

```shell
make -C rpn_cpp PLATFORM=simulator run
```

A native simulator should open on the app.

You can also debug your app using either `gdc` (or `lldb` on MacOS).
```shell
make -C rpn_cpp PLATFORM=simulator debug
```

## License

These sample apps are distributed under the terms of the BSD License. See LICENSE for details.

## Trademarks

NumWorks is a registered trademark.
