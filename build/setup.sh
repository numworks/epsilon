#!/usr/bin/env bash

# This script should be run from root epsilon folder for init_git_pre_push_hook to work

set -o errexit
set -o nounset
set -o pipefail

INSTALL_ARM_GCC=1
if [[ "${1-}" == "--only-simulator" ]]; then
  INSTALL_ARM_GCC=0
fi
if [ -x "$(command -v arm-none-eabi-gcc)" ]; then
  # We don't reinstall arm-none-eabi-gcc as the user may want to use a custom
  # version. For example, this is what the setup-arm-toolchain action will do on
  # GitHub.
  INSTALL_ARM_GCC=0
fi

install_binary_deps() {
  case $OSTYPE in
    linux*)
      install_linux_binary_deps
      install_python_deps
      ;;
    darwin*)
      install_macos_binary_deps
      install_python_deps
      ;;
    msys)
      install_windows_binary_deps
      install_windows_python_deps
      ;;
    *)
      echo "Unknown operating system $OSTYPE" >&2
      exit 1
      ;;
  esac
}

install_python_deps() {
  python3 -m venv .venv
  .venv/bin/pip3 install \
    lz4 \
    pyelftools \
    pypng \
    stringcase
}

install_windows_python_deps() {
  python3 -m venv .venv
  source .venv/Scripts/activate  # Activate the virtual environment
  pip install wheel  # Ensure you have wheel installed
  pip install lz4 --platform win_amd64  # Specify the platform name
  pip install pyelftools --platform win_amd64  # Specify the platform name
  pip install pypng --platform win_amd64  # Specify the platform name
  pip install stringcase --platform win_amd64  # Specify the platform name
}

install_macos_binary_deps() {
  if ! [ -x "$(command -v git)" ]; then
    xcode-select --install
  fi

  if ! [ -x "$(command -v brew)" ]; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  fi

  brew install \
    clang-format \
    freetype \
    imagemagick \
    libpng \
    libusb \
    pkg-config \
    python3

  if [[ "${INSTALL_ARM_GCC-0}" == "1" ]]; then
    brew install numworks/tap/arm-none-eabi-gcc
  fi
}

install_linux_binary_deps() {
  sudo apt-get install \
    build-essential \
    clang-format \
    git \
    imagemagick \
    libfreetype6-dev \
    libjpeg-dev \
    libpng-dev \
    libx11-dev \
    libxext-dev \
    pkg-config \
    python3 \
    python3-pip \
    python3-venv

  if [[ "${INSTALL_ARM_GCC-0}" == "1" ]]; then
    sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi
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

  if [[ "${INSTALL_ARM_GCC-0}" == "1" ]]; then
    echo "Please head to https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads and download the latest Windows (mingw-w64-i686) hosted cross toolchains for AArch32 bare-metal targets (arm-none-eabi)"
  fi
}

init_git_pre_push_hook() {
  # Create a git hook to prevent push on master branch without confirmation
  # See build/git/pre-push for details
  echo "Creating pre-push hook to prevent unwanted push on master."
  cp build/git/pre-push .git/hooks/pre-push
  chmod +x .git/hooks/pre-push
  git init
}

install_binary_deps
init_git_pre_push_hook
