# RPN app for Epsilon written in C

This is a basic RPN app written in C to use on a [NumWorks calculator](https://www.numworks.com).

## Run the app

To build this app on a simulator, you'll just need a C compiler (`gcc` is expected on Windows and Linux and `clang` is expected on MacOS).

```shell
make clean && make run
make debug
```

This should launch a simulator running your application (or a debugger targeting your application).

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
