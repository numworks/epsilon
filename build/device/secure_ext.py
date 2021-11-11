import sys
import os

MAGIK_CODE = [0x64, 0x6c, 0x31, 0x31, 0x23, 0x39, 0x38, 0x33, 0x35]
MAGIK_POS = [0x03, 0xb, 0x44f]

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
        first_packet[MAGIK_POS[0] + i] = MAGIK_CODE[i]
    for i in range(4):
        first_packet[MAGIK_POS[1] + i] = MAGIK_CODE[i + 5]
    for i in range(len(MAGIK_CODE)):
        first_packet[MAGIK_POS[2] + i] = MAGIK_CODE[i]

    file.seek(0)
    file.write(first_packet)
    print("External bin Patched!")