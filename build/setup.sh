#!/usr/bin/env bash

# This script should be run from root epsilon folder for init_git_pre_push_hook to work

set -o errexit
set -o nounset
set -o pipefail

INSTALL_ARM_GCC=1
if [[ "${1-}" == "--only-simulator" ]]; then
  INSTALL_ARM_GCC=0
elif [ -x "$(command -v arm-none-eabi-gcc)" ]; then
  # We don't reinstall arm-none-eabi-gcc as the user may want to use a custom
  # version. For example, this is what the setup-arm-toolchain action will do on
  # GitHub.
  INSTALL_ARM_GCC=0
fi

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
      echo "Unknown operating system $OSTYPE" >&2
      exit 1
      ;;
  esac
}

install_python_deps() {
  if [[ $OSTYPE == "msys" ]]; then
    # using system lz4 since pip install fails
    python3 -m venv .venv --system-site-packages
  else
    python3 -m venv .venv
    .venv/bin/pip3 install lz4
  fi
  .venv/bin/pip3 install \
    pyelftools \
    pypng \
    stringcase \
    black
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
    lcov \
    pkg-config \
    python3

  if [[ "${INSTALL_ARM_GCC-0}" == "1" ]]; then
    brew install numworks/tap/arm-none-eabi-gcc
  fi
}

install_linux_binary_deps() {
  # add the "debian testing" sources
  if ! [ -f "/etc/apt/sources.list.d/debian-testing.list" ]; then
    sudo bash -c "echo deb http://deb.debian.org/debian testing main >> /etc/apt/sources.list.d/debian-testing.list"
  fi
  sudo apt update && sudo apt upgrade
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
    lcov \
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
    mingw-w64-x86_64-libjpeg-turbo \
    mingw-w64-x86_64-libpng \
    mingw-w64-x86_64-librsvg \
    mingw-w64-x86_64-pkg-config \
    mingw-w64-x86_64-python3 \
    mingw-w64-x86_64-python3-lz4 \
    mingw-w64-x86_64-python3-pip

  if [[ "${INSTALL_ARM_GCC-0}" == "1" ]]; then
    pacman -S --noconfirm \
      mingw-w64-x86_64-python-pyusb
      mingw-w64-x86_64-arm-none-eabi-gcc
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

setup_nws_diff_converter() {
  # Convert .nws to .txt when displaying diff
  echo "*.nws diff=nws" >> .git/info/attributes
  git config diff.nws.textconv "python3 build/screenshots/nws_to_txt.py"
}

install_binary_deps
install_python_deps
init_git_pre_push_hook
