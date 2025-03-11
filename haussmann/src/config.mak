# Configuration variables, may be edited on the command line.

$(call assert_defined,APP_NAME)
$(call assert_defined,APP_VERSION)
$(call assert_defined,OUTPUT_ROOT)
$(call assert_defined,DEBUG)
$(call assert_defined,PLATFORM)

_platforms_device := n0110 n0115 n0120 u0-discovery
_platforms_simulator := android foxglove ios linux macos web windows

# Host detection
ifeq ($(OS),Windows_NT)
HOST := windows
else
_uname_s := $(shell uname -s)
ifeq ($(_uname_s),Darwin)
HOST := macos
else ifeq ($(_uname_s),Linux)
HOST := linux
else
HOST := unknown
endif
endif

# Providing PLATFORM=simulator will automatically select the host platform.
ifneq ($(filter simulator host,$(PLATFORM)),)
override PLATFORM := $(HOST)
endif

# PLATFORM can also be selected by defining TARGET or MODEL, for compatibility
# with the old build system of Epsilon.
ifneq ($(filter $(_platforms_device),$(MODEL)),)
override PLATFORM := $(MODEL)
endif
ifneq ($(filter $(_platforms_device) $(_platforms_simulator),$(TARGET)),)
override PLATFORM := $(TARGET)
endif

# Build type detection
ifdef ASSERTION
  ASSERTIONS := $(ASSERTION)
endif
ASSERTIONS ?= $(DEBUG)

ifeq ($(DEBUG),0)
ifeq ($(ASSERTIONS),0)
_build_type := release
else
_build_type := debug/optimized
endif
else
ifeq ($(ASSERTIONS),0)
_build_type := debug/no_assert
else
_build_type := debug
endif
endif

# Platform type detection
ifneq ($(filter $(_platforms_device),$(PLATFORM)),)
PLATFORM_TYPE := device
else
ifneq ($(filter $(_platforms_simulator),$(PLATFORM)),)
PLATFORM_TYPE := simulator
else
$(error Unsupported platform $(PLATFORM))
endif
endif

OUTPUT_DIRECTORY ?= $(OUTPUT_ROOT)/$(_build_type)/$(PLATFORM)

TOOLS_DIRECTORY ?= $(OUTPUT_ROOT)/tools

VERBOSE ?= 1
ifneq ($(VERBOSE),2)
.SILENT:
endif

# Git repository patch level
GIT := $(shell command -v git 2> /dev/null)
PATCH_LEVEL := NONE
ifdef GIT
  PATCH_LEVEL := $(shell (git rev-parse HEAD || echo NONE) | head -c 7)
endif

# ARCH is an alias for the ARCHS variable
ifdef ARCH
  ARCHS := $(ARCH)
endif

# Platform specific configuration
include $(PATH_haussmann)/src/configs/$(PLATFORM).mak
