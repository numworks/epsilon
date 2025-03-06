# RPN app for Epsilon written in C

This is a basic RPN app written in C to use on a [NumWorks calculator](https://www.numworks.com).

## Complete the application

Read the `src/main.cpp` file to understand the core structure of the application: the main loop.

### InputField

The Input Field stores the digital input of the user. Complete `Converter::Serialize` in `src/converter.cpp` and all methods of `InputField` in `src/input_field.cpp` to make it work.

### Store

The Store holds the previous values input by the user.  Complete `Converter::Parse` in `src/converter.cpp` and all methods of `Store` in `src/store.cpp` to make it work.

## Add features: make a complete RPN app!

Add code to handle multiplication, division, square root, power... And all operations you need in your RPN app.

## UX: Improve the user interface

Make this ugly app beautiful.

## Setup

To build this app on a simulator, you'll just need a C compiler (`gcc` is expected on Windows and Linux and `clang` is expected on MacOS).

```shell
./setup.sh
```

### Prepare web and native simulators

To run the apps on web or native simulators, you need to add them in an `epsilon_simulators` folder.

<!-- TODO : Complete this section -->

## Run the app

Launch the app on the platform of your choice.

### On a device

Plug in a calculator, without exiting the `CALCULATOR IS CONNECTED` screen, run
```shell
make PLATFORM=device run
```

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
