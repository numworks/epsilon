#!/usr/bin/env bash

# This script should be run from root epsilon folder for init_git_pre_push_hook to work

set -o errexit
set -o nounset
set -o pipefail

CI=0
if [[ "${1-}" == "--ci" ]]; then
  CI=1
  shift
fi

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
    cygwin)
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
    .venv/bin/pip3 install setuptools lz4
  fi

  .venv/bin/pip3 install \
    black \
    parse \
    pyelftools \
    pypng \
    pyyaml \
    rich \
    stringcase
}

install_macos_binary_deps() {
  if ! [ -x "$(command -v git)" ]; then
    xcode-select --install
  fi

  if ! [ -x "$(command -v brew)" ]; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  fi

  brew install \
    llvm \
    clang-format \
    freetype \
    imagemagick \
    libpng \
    libusb \
    lcov \
    make \
    pkg-config \
    python3

  if [[ "${INSTALL_ARM_GCC-0}" == "1" ]]; then
    brew install numworks/tap/arm-none-eabi-gcc
  fi

  setup_gnu_make
}

setup_gnu_make() {
  # macOS ships GNU Make 3.81, but the build requires >= 4.3. Homebrew installs
  # the recent version as "gmake" and exposes a "make" symlink under gnubin so
  # that "make" resolves to it when gnubin is first in PATH.
  local gnubin
  gnubin="$(brew --prefix make)/libexec/gnubin"

  if [[ $CI == "1" ]]; then
    # Make the recent "make" visible to the following steps of the GitHub job.
    echo "$gnubin" >> "$GITHUB_PATH"
  else
    echo
    echo "GNU Make >= 4.3 installed via Homebrew as \"gmake\". To make \"make\" point to it,"
    echo "add this line to your shell config (e.g. ~/.zshrc) and restart your shell:"
    echo
    echo "  export PATH=\"$gnubin:\$PATH\""
    echo
  fi
}

install_linux_binary_deps() {
  sudo apt-get update -y
  sudo apt-get install --no-upgrade -y \
    build-essential \
    ccache \
    clang-format-17 \
    git \
    imagemagick \
    libedit-dev \
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
    sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi
  fi

  if [[ $CI == "1" ]]; then
    # format.mak requires prettier
    # poincarejs workflow requires zip
    sudo apt-get install -y lcov parallel npm zip
    sudo npm install -g prettier
    sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-17 100
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
    mingw-w64-x86_64-python3

  if [[ $OSTYPE == "msys" ]]; then
    pacman -S --noconfirm \
    mingw-w64-x86_64-python3-lz4 \
    mingw-w64-x86_64-python3-pip
  fi

  if [[ "${INSTALL_ARM_GCC-0}" == "1" ]]; then
    pacman -S --noconfirm \
      mingw-w64-x86_64-python-pyusb \
      mingw-w64-x86_64-arm-none-eabi-gcc
  fi
}

init_git_pre_push_hook() {
  # Create a git hook to prevent push on master branch without confirmation
  # See tools/git/pre-push for details
  echo "Creating pre-push hook to prevent unwanted push on master."
  cp tools/git/pre-push .git/hooks/pre-push
  chmod +x .git/hooks/pre-push
  git init
}

init_git_pre_commit_hook() {
  # Create a git hook to check formatting before committing
  # See tools/git/pre-commit for details
  echo "Creating pre-commit hook to check format before committing."
  cp tools/git/pre-commit .git/hooks/pre-commit
  chmod +x .git/hooks/pre-commit
  git init
}


init_git_commit_msg_hook() {
  # Create a git hook to enforce a commit message format
  # See tools/git/commit-msg for details
  echo "Creating commit-msg hook to enforce commit messages."
  cp tools/git/commit-msg .git/hooks/commit-msg
  chmod +x .git/hooks/commit-msg
  git init
}

setup_nws_diff_converter() {
  # Convert .nws to .nwt when displaying diff
  echo "*.nws diff=nws" >> .git/info/attributes
  git config diff.nws.textconv "python3 tools/screenshots/nws_to_nwt.py"
}

install_binary_deps
install_python_deps

if [[ $CI == "0" ]]; then
  init_git_pre_push_hook
  init_git_pre_commit_hook
  init_git_commit_msg_hook
fi
