import sys
import argparse

# Script used in bob-internal:
# sullivan/fuzzer/scripts/default_format_and_move_crashes_routine.sh

parser = argparse.ArgumentParser(
    description="Add a wildcard NWS header to a headless state file."
)
parser.add_argument("src", metavar="SRC", help="input headless state file")
parser.add_argument(
    "output", metavar="OUTPUT", nargs="?", help="output file (default: overwrite src)"
)
parser.add_argument(
    "-p",
    "--project",
    choices=["epsilon", "scandium"],
    default="epsilon",
)

args = parser.parse_args()
output = args.output if args.output is not None else args.src

PROJECT_ID = {"epsilon": 1, "scandium": 2}

# NWSF**.**.**\x{project_id}**
wildCardHeader = bytearray(
    b"\x4e\x57\x53\x46\x2a\x2a\x2e\x2a\x2a\x2e\x2a\x2a"
    + bytes([PROJECT_ID[args.project]])
    + b"\x2a\x2a"
)
with open(args.src, "rb") as f:
    srcBytes = bytearray(f.read())
    newFile = open(output, "wb")
    newFile.write(wildCardHeader)
    newFile.write(srcBytes)
