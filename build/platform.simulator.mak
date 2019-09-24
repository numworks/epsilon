USE_LIBA = 0
ION_KEYBOARD_LAYOUT = layout_B2
EPSILON_GETOPT = 1

SFLAGS += -fPIE

TARGET ?= $(HOST)

BUILD_DIR := $(BUILD_DIR)/$(TARGET)

include build/platform.simulator.$(TARGET).mak
