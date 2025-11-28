import sys

src = sys.argv[1]
if len(sys.argv) > 2:
    output = sys.argv[2]
else:
    output = src

headerLength = 15
with open(src, "rb") as f:
    srcBytes = bytearray(f.read())
    newFile = open(output, "wb")
    newFile.write(srcBytes[headerLength:])
