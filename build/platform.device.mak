MODEL ?= n0110
TOOLCHAIN ?= arm-gcc-m7f
USE_LIBA = 1
EXE = elf

EPSILON_TELEMETRY ?= 0
TERMS_OF_USE ?= 0

# We want to do the following transformations:
# bootloader[./_][dfu/elf/bin/run] --> bootloader
# kernel.[A/B][./_][dfu/elf/bin/run] --> kernel
# userland.[A/B][./_][dfu/elf/bin/run]  --> userland
FIRMWARE_COMPONENT ?= $(firstword $(subst ., ,$(subst _, ,$(MAKECMDGOALS))))

ifeq ($(DEVELOPMENT),1)
EMBED_EXTRA_DATA ?= 0
else
ifndef EMBED_EXTRA_DATA
ifneq ($(filter $(FIRMWARE_COMPONENT),epsilon test bootloader kernel userland),)
# Force EMBED_EXTRA_DATA when actually building for device
$(error EMBED_EXTRA_DATA must be defined in DEVELOPMENT=0)
else
# Only display warning. This happens whith "make clean" for example.
$(warning "EMBED_EXTRA_DATA should be defined in DEVELOPMENT=0, setting it to 0.")
EMBED_EXTRA_DATA ?= 0
endif
endif
endif

BUILD_DIR := $(BUILD_DIR)/$(MODEL)/$(FIRMWARE_COMPONENT)

$(BUILD_DIR)/python/port/port.o: CXXFLAGS += -DMP_PORT_USE_STACK_SYMBOLS=1

include build/platform.device.$(MODEL).mak
