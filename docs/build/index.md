# Build and run your own version of Epsilon

## Install the SDK

### Windows

We recommend using the [Msys2](https://www.msys2.org/) environment to install most of the required tools. We support Windows 7 and up. Once Msys2 has been installed, launch the Msys2 terminal application, and enter the following commands

```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freetype mingw-w64-x86_64-pkg-config mingw-w64-x86_64-libusb make mingw-w64-x86_64-python3 mingw-w64-x86_64-libjpeg-turbo mingw-w64-x86_64-libpng mingw-w64-x86_64-imagemagick mingw-w64-x86_64-librsvg mingw-w64-x86_64-inkscape mingw-w64-x86_64-python3-pip
echo "export PATH=/mingw64/bin:$PATH" >> .bashrc
pip3 install lz4 pypng stringcase
```

Last but not least, download and install the latest [GCC toolchain from ARM](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads). When prompted for an install location, choose `C:\msys64\home\User\gcc-arm\`. You'll then need to add this folder to your \$PATH in Msys2 by running this command: `echo "export PATH=$PATH:$HOME/gcc-arm/bin" >> .bashrc` and restarting Msys2.

### macOS

We recommend using [Homebrew](https://brew.sh) to install all dependencies. Once you have installed Homebrew, install all the dependencies with the following command:

```
brew install numworks/tap/epsilon-sdk
```

### Debian or Ubuntu

Most of the required tools are available as apt packages:

```
apt-get install build-essential git imagemagick libx11-dev libxext-dev libfreetype6-dev libpng-dev libjpeg-dev pkg-config python3 python3-pip
pip3 install lz4 pypng stringcase
```

You'll also need to install the latest version of GCC and make it available in your \$PATH:

1. Download the [GCC toolchain distributed by ARM](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads). You should obtain a `gcc-arm-none-eabi-x-linux.tar.bz2` file.
2. Decompress that file with `tar xvfj gcc-arm-none-eabi-*-linux.tar.bz2`
3. Add the resulting folder to your \$PATH. If you use bash, ``echo "export PATH=\$PATH:`find $(pwd)/gcc-arm-none-eabi-*-update/bin -type d`" >> ~/.bashrc`` should do what you need (you'll need to restart your terminal afterwards).

Alternatively, on Debian 10 and later you can directly install a sufficiently modern cross-toolchain:
```
apt-get install gcc-arm-none-eabi binutils-arm-none-eabi
```

## Retrieve the source code

The code is hosted on <a href="https://github.com/numworks/epsilon">GitHub</a>. You can retrieve it using the following command in your terminal (Msys2, Terminal.app, xterm…).

```
git clone https://github.com/numworks/epsilon.git
```

## Run Epsilon on your computer

Once the SDK has been installed, just open your terminal, navigate to the epsilon folder and type the following commands:

```
make PLATFORM=simulator clean
make PLATFORM=simulator epsilon_run
```

## Run Epsilon on your calculator as a third-party software

You can also update your NumWorks calculator easily. Please note that after a reset, your calculator will exclusively run an official NumWorks software.

You'll first need to plug your calculator to the computer via USB, and check that there is a message "The calculator is connected" displayed on the calculator screen.
Then open your terminal, navigate to the epsilon folder and type the following commands:
```
make clean
if [ "$(python3 build/device/dfu.py -l | grep '0x90000000')" ]; then make userland.A.dfu; python3 build/device/dfu.py -s 0x90010000:leave -D output/release/device/n0110/userland/userland.A.dfu; else make userland.B.dfu; python3 build/device/dfu.py -s 0x90410000:leave -D output/release/device/n0110/userland/userland.B.dfu; fi;
```

Congratulations, you're running your very own version of Epsilon!

## Troubleshooting

If you encounter an error when flashing the device (`python3 build/device/dfu.py -s 0x90010000:leave -D ...`), make sure you've closed all web pages that could interact with the calculator (such as https://my.numworks.com/).
If your device is not detected (`python3 build/device/dfu.py -l` failing), try with another cable and another port of your computer.
