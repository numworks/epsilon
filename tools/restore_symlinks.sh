#!/usr/bin/env bash

cd epsilon
rm .venv tools
ln -s ../.venv .venv
ln -s ../tools tools

cd ../poincare/poincarejs
rm .emsdk-version
ln -s ../../.emsdk-version .emsdk-version
