# THIRD-PARTY USERLAND

The external flash is divided in two slots that we call A and B.
At least, one of the slot holds the official firmware.
We can know which slot is running by reading the first 16 bytes of the sRAM by doing for instance:
```cpp
dfu-util -a 1 -s 0x20000000:16:force -U slot_info.bin
```

The slot info format is the following:
- A magic prefix 0xBADBEEEF
- the address of the kernel header of the currently run kernel
- the address of the userland header of the currently run userland
- A magic suffix 0xBADBEEEF

After booting on the official software, a third-party userland can be flashed anywhere in the non-used slot.
The userland should be prefixed by a header with the following format:
- a magic prefix 0xFEEDC0DE
- the expected epsilon version (that indicates which kernel version the third-party userland is intended to)
- the address of the storage in sRAM
- the size of the storage in sRAM
- the address of the storage in Flash
- the end address of the storage in Flash
- a magic suffix 0xFEEDC0DE
- the address of the process stack start
- the address of the entry point
