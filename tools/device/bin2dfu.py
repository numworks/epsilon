# Convert binary file into DfuSe file

import argparse
import os
from dfu_helper import generate_dfu_file


def bin2dfu(bin_file, address, usb_vid_pid, dfu_file, verbose, altsetting_offset):
    # Read binary file
    with open(bin_file, "rb") as f:
        data = f.read()

    if verbose:
        print("--- Binary to DFU ---")
        print(f"\tInput file: {bin_file}")
        print(f"\tData size: {len(data)} ({hex(len(data))})")
        print(f"\tAddress: {hex(address)}")
        print(f"\tOutput file: {dfu_file}")
        print()

    # Create a single target with the binary data
    targets = [{"address": address, "data": data}]

    # Generate DFU file
    generate_dfu_file([targets], usb_vid_pid, dfu_file, altsetting_offset)


parser = argparse.ArgumentParser(
    description="Convert a binary file to DfuSe at a specified address."
)
parser.add_argument("-i", metavar="BIN_FILE", help="Input binary file", required=True)
parser.add_argument("-s", metavar="ADDRESS", help="Target address", required=True)
parser.add_argument("-o", metavar="DFU_FILE", help="Output DfuSe file", required=True)
parser.add_argument(
    "-u", metavar="USB_VID_PID", help="Target device VID:PID", required=True
)
parser.add_argument("-v", "--verbose", action="store_true", help="Show verbose output")
parser.add_argument(
    "-a",
    metavar="ALTERNATE",
    type=int,
    default=0,
    help="Alternate setting numbering",
)

args = parser.parse_args()

# Parse address (supports hex with 0x prefix)
address = int(args.s, 0)

bin2dfu(args.i, address, args.u, args.o, args.verbose, args.a)
