TOOLCHAIN ?= host-gcc
USE_LIBA ?= 0
ION_KEYBOARD_LAYOUT = layout_B2
EXE = bin

ifeq ($(DEBUG),1)
else
SFLAGS += -DNDEBUG
endif
