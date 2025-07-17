#!/bin/bash

# Try to read version from ../.emsdk-version
if [ -f ../.emsdk-version ]; then
  EMSDK_VERSION=$(cat ../.emsdk-version)
else
  # Fallback to default version if file does not exist
  EMSDK_VERSION="latest"
fi

git clone https://github.com/emscripten-core/emsdk.git
./emsdk/emsdk install $EMSDK_VERSION
./emsdk/emsdk activate $EMSDK_VERSION
