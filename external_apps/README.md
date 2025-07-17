# External apps

This folder contains apps to run on a [NumWorks calculator](https://www.numworks.com).

- [rpn_cpp](rpn_cpp) : a RPN engine to be completed
- [sample_c](sample_c) : a C application plotting squares and using external data
- [sample_cpp](sample_cpp) : a space invaders game
- [sample_rust](sample_rust) : not C/C++ based, following setup instructions do not apply, see [sample_rust instructions](sample_rust/README.md).

Following instructions are to be executed from [this](./) folder.
They will guide you to install the app [sample_cpp](sample_cpp) app on a Numworks calculator or on a simulator.

You can swap `sample_cpp` for any other C/C++ based external app (`rpn_cpp` or `sample_c`).

## Setup

As prerequisite, you will need :
- A `C` compiler (`gcc` is expected on Windows and Linux and `clang` is expected on MacOS). You can run [Epsilon's setup script](../build/setup.sh) for that
- [`npm`](https://docs.npmjs.com/downloading-and-installing-node-js-and-npm) installed

Then, you'll need to install `nwlink` on your computer.
```shell
npm install -g nwlink
```

## Install the app on your calculator

This will build and load your app on a plugged in calculator.

The calculator must be on the `CALCULATOR IS CONNECTED` screen.
![Calculator is connected screenshot](docs/calculator_connected.png)

```shell
make -C sample_cpp PLATFORM=device run
```

A `THIRD PARTY APPLICATIONS` message will appear, and the app should be accessible at the end of the Home menu.

![Third party application screenshot](docs/third_party_applications.png)

### Distribute your `.nwa` app

After a run or a build
```shell
make -C sample_cpp PLATFORM=device build
```

You should have a `sample_cpp/output/device/rpn.nwa` file that you can distribute!

Anyone can now install it on their calculator from the [NumWorks online uploader](https://my.numworks.com/apps).

## Try your app on an Epsilon simulator

An a simulator, you can test and debug your app more easily.

#### Prepare web and native simulators

By default, an [epsilon_simulators](epsilon_simulators/) folder is expected.

It should contain the expected platform-specific Epsilon simulators to run any external app on.
See instructions to build them from [Epsilon's root](../) :
- [Web](epsilon_simulators/web/README.md) : `epsilon.html`
- [Linux](epsilon_simulators/linux/README.md) : `epsilon.bin`
- [MacOS](epsilon_simulators/macos/README.md) : `epsilon.app/Contents/MacOS/Epsilon`
- [Windows](epsilon_simulators/windows/README.md) : `epsilon.exe`

Simulator path can also be overridden with the `SIMULATOR=[PATH_TO_SIMULATOR]` compilation flag.

### Native simulator (MacOS, Linux, Windows)

```shell
make -C sample_cpp PLATFORM=simulator run
```

A native simulator should open on the app.

You can also debug your app using either `gcd` (or `lldb` on MacOS).
```shell
make -C sample_cpp PLATFORM=simulator debug
```

### Web simulator

#### Additional Setup

Depending on you system :
- Windows :
```shell
./setup_web.ps1
```
- Others :
```shell
chmod +x setup_web.sh & ./setup_web.sh
```

#### Launch a server

On a separate shell, run
```shell
make -C sample_cpp server
```

#### Build and run your app

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
make -C sample_cpp PLATFORM=web run
```

Your browser should display the simulator with the app inside.

## License

These apps are distributed under the terms of the BSD License. See LICENSE for details.

## Trademarks

NumWorks is a registered trademark.
