---
title: Installing the SDK &mdash; Software Engineering
layout: breadcrumb
breadcrumb: SDK
---
# Build and run your own version of Epsilon

## Install the SDK

### Windows

We recommend using the [Msys2](https://www.msys2.org/) environment to install all the required tools. We support Windows 7 and up. Once Msys2 has been installed, launch the Msys2 terminal application, and enter the following commands

```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freetype mingw-w64-x86_64-pkg-config mingw-w64-x86_64-fltk git make bison python
export PATH=/mingw64/bin:$PATH
```

### macOS

We recommend using [Homebrew](https://brew.sh) to install most dependencies. Once you have installed Homebrew, install all the dependencies with the following command:

```
brew reinstall fltk freetype dfu-util libpng pkg-config
brew cask reinstall gcc-arm-embedded
```

### Debian or Ubuntu

Installing the required tools is just a command away:

```
apt-get install bison build-essential dfu-util flex gcc-arm-none-eabi git libfltk1.3-dev libfreetype6-dev libpng12-dev
```

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

```
make clean
make
make epsilon_flash
```

## What's next
Congratulations, you've built your first firmware! If you want to dive deeper into the code, you can either read about our <a href="../embedded/">design considerations</a> or discover <a href="../architecture/">Epsilon's architecture</a>.
