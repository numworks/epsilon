CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
LD = arm-none-eabi-ld.bfd
GDB = arm-none-eabi-gdb
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

ifeq ($(DEBUG),1)
SFLAGS += -ggdb3
else
SFLAGS += -fdata-sections -ffunction-sections
endif
SFLAGS += -mfloat-abi=hard -mcpu=arm926ej-s
LDFLAGS += -L/usr/lib/gcc/arm-none-eabi/5.4.1/fpu -lgcc

ARCH = armv5
