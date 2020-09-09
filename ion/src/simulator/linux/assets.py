# This script generates a .s file representing jpg assets in order to access
# them from C code

import sys
import re
import argparse
import io

parser = argparse.ArgumentParser(description="Process some jpg files.")
parser.add_argument('--files', nargs='+', help='a list of jpg file names')
parser.add_argument('--implementation', help='the .s file to generate')
args = parser.parse_args()

def print_jpg(f, jpg):
    f.write(".global _ion_simulator_" + jpg + "_start\n")
    f.write(".global _ion_simulator_" + jpg + "_end\n")
    f.write("_ion_simulator_" + jpg + "_start:\n")
    f.write("    .incbin \"ion/src/simulator/assets/" + jpg + ".jpg\"\n")
    f.write("_ion_simulator_" + jpg + "_end:\n\n")

def print(files, path):
    f = open(path, "w")
    for jpg in files:
        print_jpg(f, jpg)

    f.close()

print(args.files, args.implementation)

