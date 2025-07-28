#!/bin/bash

EMSDK_VERSION=$(cat .emsdk-version)
CURRENT_DIR="$(pwd)"

echo "Installing Emscripten SDK version: $EMSDK_VERSION"
cd $EMSDK
./emsdk install $EMSDK_VERSION
./emsdk activate
source ./emsdk_env.sh
cd $CURRENT_DIR
echo "Emscripten SDK installed and activated."
