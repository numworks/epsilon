CC := clang
CXX := clang++
LD := clang
GDB := arm-none-eabi-gdb
OBJCOPY := arm-none-eabi-objcopy

EXECUTABLE_EXTENSION := elf

COMPILER_FAMILY := clang

DFU ?= dfu-util

CFLAGS += -target $(TARGET) $(MCUFLAGS) -fshort-enums
CXXFLAGS += -target $(TARGET) $(MCUFLAGS) -fshort-enums -Wno-invalid-constexpr -Wno-inline-new-delete

LDFLAGS += $(MCUFLAGS) -lgcc

SFLAGS += \
  -fno-builtin \
  -fdata-sections \
  -ffreestanding \
  -ffunction-sections \
  -ggdb3 \
  -nostdinc \
  -nostdlib

ifeq ($(DEBUG),0)
LDFLAGS += -Wl,--gc-sections
endif
