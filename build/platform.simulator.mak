USE_LIBA = 0
ION_KEYBOARD_LAYOUT = layout_B2
EPSILON_GETOPT = 1

SFLAGS += -fPIE

TARGET ?= $(HOST)

BUILD_DIR := $(BUILD_DIR)/$(TARGET)

EPSILON_SIMULATOR_HAS_LIBPNG ?= 0

include build/platform.simulator.$(TARGET).mak

SFLAGS += -DEPSILON_SIMULATOR_HAS_LIBPNG=$(EPSILON_SIMULATOR_HAS_LIBPNG)

ifeq ($(EPSILON_SIMULATOR_HAS_LIBPNG),1)
SFLAGS += $(shell libpng-config --cflags)
LDFLAGS += $(shell libpng-config --ldflags)
endif
