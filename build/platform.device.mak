MODEL ?= n0110
TOOLCHAIN ?= arm-gcc-m7f
USE_LIBA = 1
EXE = elf

EPSILON_TELEMETRY ?= 0
EMBED_EXTRA_DATA ?= 0

# We want to do the following transformations:
# bootloader[./_][dfu/elf/bin/run] --> bootloader
# kernel.[A/B][./_][dfu/elf/bin/run] --> kernel
# userland.[A/B][./_][dfu/elf/bin/run]  --> userland
FIRMWARE_COMPONENT ?= $(firstword $(subst ., ,$(subst _, ,$(MAKECMDGOALS))))

BUILD_DIR := $(BUILD_DIR)/$(MODEL)/$(FIRMWARE_COMPONENT)

$(BUILD_DIR)/python/port/port.o: CXXFLAGS += -DMP_PORT_USE_STACK_SYMBOLS=1

include build/platform.device.$(MODEL).mak
