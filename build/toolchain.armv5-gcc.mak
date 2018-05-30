CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
LD = arm-none-eabi-ld.bfd
GDB = arm-none-eabi-gdb
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

LIBGCC_LOCATION ?= /usr/lib/gcc/arm-none-eabi/5.4.1

ifeq ($(DEBUG),1)
SFLAGS += -ggdb3
else
SFLAGS += -fdata-sections -ffunction-sections
endif
SFLAGS += -mfloat-abi=soft -mcpu=arm926ej-s
LDFLAGS += -L$(LIBGCC_LOCATION) -lgcc

ARCH = armv5
