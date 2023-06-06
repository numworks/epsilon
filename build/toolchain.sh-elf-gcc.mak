CC = sh-elf-gcc
CXX = sh-elf-g++
LD = sh-elf-g++
GDB = gdb
OBJCOPY = sh-elf-objcopy
SIZE = sh-elf-size
AS = sh-elf-as
FXGXA = fxgxa

SFLAGS += -D_FXCG -D_BIG_ENDIAN  -DSTRING_STORAGE
