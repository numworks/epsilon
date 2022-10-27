#!/usr/bin/env bash
set -o errexit
set -o nounset
set -o pipefail

install_binary_deps() {
  case $OSTYPE in
    linux*)
      install_linux_binary_deps
      ;;
    darwin*)
      install_macos_binary_deps
      ;;
    msys)
      install_windows_binary_deps
      ;;
    *)
      echo "Unknown operating system $OSTYPE"
      exit -1
      ;;
  esac
}

install_python_deps() {
  python3 -m venv .venv
  .venv/bin/pip3 install pypng lz4 stringcase
}

install_macos_binary_deps() {
  echo "Installing binary dependencies for macOS"

  if ! [ -x "$(command -v git)" ]; then
    xcode-select --install
  fi

  if ! [ -x "$(command -v brew)" ]; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  fi

  brew install \
    freetype \
    imagemagick \
    libpng \
    libusb
    numworks/tap/arm-none-eabi-gcc \
    pkg-config \
    python3 \
}

install_linux_binary_deps() {
  apt-get install \
    build-essential \
    git \
    imagemagick \
    libfreetype6-dev \
    libjpeg-dev \
    libpng-dev \
    libx11-dev \
    libxext-dev \
    pkg-config \
    python3 \
    python3-pip

  if ! [ -x "$(command -v arm-none-eabi-gcc)" ]; then
    # We don't automatically install those packages as the user may want to use
    # a more recent version. For example, this is what the setup-arm-toolchain
    # action will do on GitHub.
    apt-get install gcc-arm-none-eabi binutils-arm-none-eabi
  fi
}

install_windows_binary_deps() {
  # Make sure we're in the proper msys2 shell?
  # "uname" should yield MINGW64
  pacman -S --noconfirm \
    make \
    mingw-w64-x86_64-freetype \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-imagemagick \
    mingw-w64-x86_64-inkscape \
    mingw-w64-x86_64-libjpeg-turbo \
    mingw-w64-x86_64-libpng \
    mingw-w64-x86_64-librsvg \
    mingw-w64-x86_64-pkg-config \
    mingw-w64-x86_64-python3 \
    mingw-w64-x86_64-python3-pip
}

install_binary_deps
install_python_deps
