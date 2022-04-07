import sys
import os

MAGIK_CODE = [0x32, 0x30, 0x30, 0x36]
MAGIK_POS = 0x44F
# Disable Script
sys.exit(0)

if len(sys.argv) > 1:
    ext_path = os.path.join(os.getcwd(), sys.argv[1])
    if not os.path.isfile(ext_path):
        print("Error: File not found!")
        sys.exit(-1)
    file = open(ext_path, "r+b")
    file.read(MAGIK_POS)
    packet = bytearray(file.read(4))
    for b in packet:
        if b != 0:
            print("Error: Invalid file! (maybe already patched?)")
            sys.exit(-1)
    
    for i in range(4):
        packet[i] = MAGIK_CODE[i]

    file.seek(MAGIK_POS)
    file.write(packet)
    print("External bin patched!")