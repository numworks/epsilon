USE_LIBA = 0
EPSILON_GETOPT = 1

SFLAGS += -fPIE

TARGET ?= $(HOST)

COVERAGE =
BUILD_DIR := $(BUILD_DIR)/$(TARGET)

ifneq ($(findstring coverage,$(MAKECMDGOALS)),)
COVERAGE = coverage
BUILD_DIR := $(BUILD_DIR)/$(COVERAGE)
endif


include build/platform.simulator.$(TARGET).mak
