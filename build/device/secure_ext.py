import sys
import os

MAGIK_CODE = [0x32, 0x30, 0x30, 0x36]
MAGIK_POS = 0x44F

if len(sys.argv) > 1:
    print("Patching external bin...")
    ext_path = os.path.join(os.getcwd(), sys.argv[1])
    if not os.path.isfile(ext_path):
        print("Error: File not found!")
        sys.exit(-1)
    file = open(ext_path, "r+b")
    first_packet = bytearray(file.read(2048))
    for b in first_packet:
        if b != 255:
            print("Error: Invalid file! (maybe already patched?)")
            sys.exit(-1)
    
    for i in range(4):
        first_packet[MAGIK_POS + i] = MAGIK_CODE[i]

    file.seek(0)
    file.write(first_packet)
    print("External bin Patched!")