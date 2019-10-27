CC = clang
CXX = clang++
LD = arm-none-eabi-ld.bfd
GDB = arm-none-eabi-gdb
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

SFLAGS += -target thumbv7em-unknown-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16
ifeq ($(DEBUG),1)
SFLAGS += -ggdb3
else
SFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += --gc-sections
endif
