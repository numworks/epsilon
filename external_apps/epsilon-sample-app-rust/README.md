# Sample Rust app for Epsilon

[![Build](https://github.com/numworks/epsilon-sample-app-rust/actions/workflows/build.yml/badge.svg)](https://github.com/numworks/epsilon-sample-app-rust/actions/workflows/build.yml)

<img src="/doc/screenshots.gif?raw=true" alt="Sample Rust app for the NumWorks graphing calculator" width="300" align="right">

This is a sample [Rust](https://www.rust-lang.org) app to use on a [NumWorks calculator](https://www.numworks.com). Yes, you can now use Rust to write code for a graphing calculator!

```rust
fn eadk_main() {
    for _ in 0..100 {
        let c = eadk::Color { rgb565: random_u16() };
        let r = eadk::Rect { x: random_coordinate(), y: random_coordinate(), width: random_coordinate(), height: random_coordinate() };
        eadk::display::push_rect_uniform(r, c);
    }
    loop {}
}
```

## Build the app

To build this sample app, you will need to install an embedded ARM rust compiler as well as [Node.js](https://nodejs.org/en/). The SDK for Epsilon apps is shipped as an npm module called [nwlink](https://www.npmjs.com/package/nwlink) that will automatically be installed at compile time.

```shell
brew install rustup node # Or equivalent on your OS
rustup-init
rustup target add thumbv7em-none-eabihf
cargo build
```

## Run the app

The app is sent over to the calculator using the DFU protocol over USB.

```shell
# Now connect your NumWorks calculator to your computer using the USB cable
cargo run
```

## Notes

The NumWorks calculator runs [Epsilon](http://github.com/numworks/epsilon), a tailor-made calculator operating system. Starting from version 16, Epsilon allows installing custom binary apps. To run this sample app, make sure your calculator is up-to-date by visiting https://my.numworks.com.

Due to the embedded nature of Epsilon, the Rust app has to be `no_std`. The interface that an app can use to interact with the OS is essentially a short list of system calls. Feel free to browse the [code of Epsilon](http://github.com/numworks/epsilon) itself if you want to get an in-depth look.

Please note that any custom app is removed when resetting the calculator.

## License

This sample app is distributed under the terms of the BSD License. See LICENSE for details.

## Trademarks

NumWorks and Rust are a registered trademarks.
