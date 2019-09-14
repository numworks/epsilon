CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
LD = arm-none-eabi-gcc
GDB = arm-none-eabi-gdb
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Always generate debug information
SFLAGS += -ggdb3

# Put data/code symbols in their own subsection
# This allows the linker script to precisely place a given symbol
SFLAGS += -fdata-sections -ffunction-sections

ifeq ($(DEBUG),1)
LTO ?= 0
else
LTO ?= 1
endif

ifeq ($(LTO),1)
# Use link-time optimization if LTO=1
SFLAGS += -flto
else
# Otherwise, just get rid of unused symbols
LDFLAGS += -Wl,--gc-sections
endif

LDFLAGS += $(SFLAGS) -lgcc -Wl,-T,$(LDSCRIPT)

# To debug linker scripts, add the following line
# LDFLAGS += -Wl,-M
