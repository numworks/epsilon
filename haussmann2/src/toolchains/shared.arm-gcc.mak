CC := arm-none-eabi-gcc
CXX := arm-none-eabi-g++
AR := arm-none-eabi-gcc-ar
LD := arm-none-eabi-g++
GDB := arm-none-eabi-gdb

OBJCOPY := arm-none-eabi-objcopy

EXECUTABLE_EXTENSION := elf

SFLAGS += \
  -fdata-sections \
  -ffreestanding \
  -ffunction-sections \
  -ggdb3 \
  -nostdinc \
  -nostdlib \
  -Wl,--whole-archive

ifeq ($(DEBUG),0)
SFLAGS += -flto=auto
endif

LDFLAGS := \
  -Wl,--no-whole-archive \
  $(LDFLAGS) \
  -lgcc \
  -Wl,--gc-sections
