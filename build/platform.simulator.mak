USE_LIBA = 0
EPSILON_GETOPT = 1

SFLAGS += -fPIE

TARGET ?= $(HOST)

COVERAGE =
ifneq ($(findstring coverage,$(MAKECMDGOALS)),)
COVERAGE = coverage
endif

BUILD_DIR := $(BUILD_DIR)/$(TARGET)/$(COVERAGE)

include build/platform.simulator.$(TARGET).mak
