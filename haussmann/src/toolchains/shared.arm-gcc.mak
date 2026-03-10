CC := arm-none-eabi-gcc
CXX := arm-none-eabi-g++
LD := arm-none-eabi-g++
GDB := arm-none-eabi-gdb

OBJCOPY := arm-none-eabi-objcopy

EXECUTABLE_EXTENSION := elf

COMPILER_FAMILY := gcc

DFU ?= dfu-util

SFLAGS += \
  -fdata-sections \
  -ffreestanding \
  -ffunction-sections \
  -ggdb3 \
  -nostdinc \
  -nostdlib

ifeq ($(DEBUG),0)
SFLAGS += -flto=auto
endif

LDFLAGS += \
  -lgcc \
  -Wl,--gc-sections
