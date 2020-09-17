# This script generates a .s file representing assets in order to access
# them from C code

import sys
import re
import argparse
import io
import os

parser = argparse.ArgumentParser(description="Process some asset files.")
parser.add_argument('--files', nargs='+', help='a list of file names')
parser.add_argument('--implementation', help='the .s file to generate')
args = parser.parse_args()

def print_asset(f, asset):
    asset_basename = os.path.splitext(asset)[0]
    f.write(".global _ion_simulator_" + asset_basename + "_start\n")
    f.write(".global _ion_simulator_" + asset_basename + "_end\n")
    f.write("_ion_simulator_" + asset_basename + "_start:\n")
    f.write("    .incbin \"ion/src/simulator/assets/" + asset + "\"\n")
    f.write("_ion_simulator_" + asset_basename + "_end:\n\n")

def print(files, path):
    f = open(path, "w")
    for asset in files:
        print_asset(f, asset)

    f.close()

print(args.files, args.implementation)

