---
title: Installing the SDK &mdash; Software Engineering
layout: breadcrumb
breadcrumb: SDK
---
# Build and run your own version of Epsilon

## Install the SDK

### Windows

We recommend using the [Msys2](https://www.msys2.org/) environment to install most of the required tools. We support Windows 7 and up. Once Msys2 has been installed, launch the Msys2 terminal application, and enter the following commands

```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freetype mingw-w64-x86_64-pkg-config mingw-w64-x86_64-fltk git make python
echo "export PATH=/mingw64/bin:$PATH" >> .bashrc
```

Last but not least, download and install the latest [GCC toolchain from ARM](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads). When prompted for an install location, choose `C:\msys64\home\User\gcc-arm\`. You'll then need to add this folder to your $PATH in Msys2 by running this command: `echo "export PATH=$PATH:$HOME/gcc-arm/bin" >> .bashrc` and restarting Msys2.

### macOS

We recommend using [Homebrew](https://brew.sh) to install all dependencies. Once you have installed Homebrew, install all the dependencies with the following command:

```
brew install armmbed/formulae/arm-none-eabi-gcc fltk freetype libpng pkg-config python
```

### Debian or Ubuntu

Most of the required tools are available as apt packages:

```
apt-get install build-essential git libfltk1.3-dev libfreetype6-dev libpng-dev pkg-config
```

You'll also need to install the latest version of  and make it available in your $PATH:

1. Download the [GCC toolchain distributed by ARM](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads). You should obtain a `gcc-arm-none-eabi-x-linux.tar.bz2` file.
2. Decompress that file with `tar xvfj gcc-arm-none-eabi-*-linux.tar.bz2`
3. Add the resulting folder to your $PATH. If you use bash, ``echo "export PATH=\$PATH:`find $(pwd)/gcc-arm-none-eabi-*-update/bin -type d`" >> ~/.bashrc`` should do what you need (you'll need to restart your terminal afterwards).

## Retrieve the source code

The code is hosted on <a href="https://github.com/numworks/epsilon">GitHub</a>. You can retrieve it using the follwing command.

```
git clone https://github.com/numworks/epsilon.git
```

## Run Epsilon on your computer

Once the SDK has been installed, just open your terminal (Msys2, Terminal.app, xterm, etc...) and type the following commands:

```
make PLATFORM=simulator clean
make PLATFORM=simulator
./epsilon.elf
```

## Run Epsilon on your calculator

You can also update your NumWorks calculator easily. Note that you'll need to press the Reset button and that all data on your calculator will be lost.

If you got the hardware calculator some time ago, chances are you have a 'N0100' model. If you already compiled/flashed epsilon in 11.x.x branches or lower without telling your model, this should be what you have. The default model changed since, and you should do:

```
make clean
make MODEL=n0100
make MODEL=n0100 epsilon_flash
```

If your calculator is more recent, you probably have the 'N0110' model. You should use:

```
make clean
make
make epsilon_flash
```

Trying to flash the wrong version on your calculator will probably not work, resulting in a stuck calculator (with a black screen). You can recover from that by flashing the correct version: you can plug your calculator and follow the instructions (press the RESET button and maybe 6 at the same time) even if the flash failed, because the DFU mode is never broken.

Congratulations, you're running your very own version of Epsilon!
