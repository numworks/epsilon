# Sample Rust app for Epsilon

<img src="doc/screenshots.gif?raw=true" alt="Sample Rust app for the NumWorks graphing calculator" width="300" align="right">

This is a sample [Rust](https://www.rust-lang.org) app to use on a [NumWorks calculator](https://www.numworks.com).

Yes, you can now use Rust to write code for a graphing calculator!

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

## Requirements

[Rust](https://www.rust-lang.org/tools/install) and [Node.js](https://nodejs.org/en/).

To build this sample app, you will need to install an embedded ARM rust compiler:

```shell
rustup target add thumbv7em-none-eabihf
```

The SDK for Epsilon apps is shipped as an npm module called [nwlink](https://www.npmjs.com/package/nwlink) that will automatically be installed at compile time.

## Build the app

```shell
cargo build
```

## Run the app

The app is sent over to the calculator using the DFU protocol over USB,
your calculator needs to be plugged in and on the `CALCULATOR IS CONNECTED` screen.

```shell
cargo run
```

## Notes

The NumWorks calculator runs [Epsilon](http://github.com/numworks/epsilon), a tailor-made calculator operating system.
To run this sample app, make sure your calculator is up-to-date by visiting https://my.numworks.com.

Due to the embedded nature of Epsilon, the Rust app has to be `no_std`.
The interface that an app can use to interact with the OS is essentially a short list of system calls.
Feel free to browse the [code of Epsilon](http://github.com/numworks/epsilon) itself if you want to get an in-depth look.

Please note that any custom app is removed when resetting the calculator.
