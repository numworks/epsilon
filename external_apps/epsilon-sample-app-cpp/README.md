# Sample C++ app for Epsilon

[![Build](https://github.com/numworks/epsilon-sample-app-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/numworks/epsilon-sample-app-cpp/actions/workflows/build.yml)

<img src="/doc/screenshots.gif?raw=true" alt="Sample C++ app for the NumWorks graphing calculator" width="300" align="right">

This is a sample C++ app to use on a [NumWorks calculator](https://www.numworks.com).

```cpp

using namespace EADK;

void eadk_main() {
  Display::pushRectUniform(
    Display::Rect(0, 0, 320, 240),
    Display::Color(0x000000)
  );
  while (1) {
    Keyboard::State kbd = Keyboard::scan();
    if (kbd.keyDown(Keyboard::Key::OK)) {
      spaceship.createRockets();
    }
    if (kbd.keyDown(Keyboard::Key::Up)) {
      spaceship.move(0, -Spaceship::k_step);
    }
    refreshScene();
  }
}
```

## Build the app

To build this sample app, you will need to install the [embedded ARM toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) and [Node.js](https://nodejs.org/en/). The C SDK for Epsilon apps is shipped as an npm module called [nwlink](https://www.npmjs.com/package/nwlink) that will automatically be installed at compile time.

```shell
brew install numworks/tap/arm-none-eabi-gcc node # Or equivalent on your OS
make
```

You should now have a `target/voord.nwa` file that you can distribute! Anyone can now install it on their calculator from the [NumWorks online uploader](https://my.numworks.com/apps).

## Run the app locally

To run the app on your development machine, you can use the following command

```shell
# Now connect your NumWorks calculator to your computer using the USB cable
make run
```

## License

This sample app is distributed under the terms of the BSD License. See LICENSE for details.

## Trademarks

NumWorks is a registered trademark.
