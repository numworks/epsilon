import sys

# Script used in bob-internal:
# sullivan/fuzzer/scripts/default_format_and_move_crashes_routine.sh

src = sys.argv[1]
if len(sys.argv) > 2:
    output = sys.argv[2]
else:
    output = src

# NWSF**.**.**\x01**"
wildCardHeader = bytearray(
    b"\x4e\x57\x53\x46\x2a\x2a\x2e\x2a\x2a\x2e\x2a\x2a\x01\x2a\x2a"
)
with open(src, "rb") as f:
    srcBytes = bytearray(f.read())
    newFile = open(output, "wb")
    newFile.write(wildCardHeader)
    newFile.write(srcBytes)
