# coding=utf-8

import argparse
import dfu
import os.path


def print_layout(file):
    if os.path.splitext(file)[1] != ".dfu":
        raise ValueError("This file is not a .dfu")
    dfu.read_dfu_file(file)


parser = argparse.ArgumentParser(
    description="Print the layout of the sections of a DFU file."
)
parser.add_argument("input", metavar="DFU_FILE", help="Input DfuSe")

args = parser.parse_args()
print_layout(args.input)
