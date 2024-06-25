# Configuration variables, may be edited on the command line.

$(call assert_defined,APP_NAME)
$(call assert_defined,APP_VERSION)
$(call assert_defined,OUTPUT_ROOT)
$(call assert_defined,DEBUG)
$(call assert_defined,PLATFORM)

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

ifneq ($(filter simulator host,$(PLATFORM)),)
override PLATFORM := $(HOST)
endif

ifeq ($(DEBUG),0)
_build_type := release
else
_build_type := debug
endif

OUTPUT_DIRECTORY ?= $(OUTPUT_ROOT)/$(_build_type)/$(PLATFORM)

TOOLS_DIRECTORY ?= $(OUTPUT_ROOT)/tools

VERBOSE ?= 0
ifeq ($(VERBOSE),0)
QUIET := @
endif

# Platform type detection
_platforms_device := n0110 n0115 n0120
_platforms_simulator := android foxglove ios linux macos web windows
ifneq ($(filter $(_platforms_device),$(PLATFORM)),)
PLATFORM_TYPE := device
else
ifneq ($(filter $(_platforms_simulator),$(PLATFORM)),)
PLATFORM_TYPE := simulator
else
$(error Unsupported platform $(PLATFORM))
endif
endif

# Git repository patch level
GIT := $(shell command -v git 2> /dev/null)
PATCH_LEVEL = NONE
ifdef GIT
  PATCH_LEVEL = $(shell (git rev-parse HEAD || echo NONE) | head -c 7)
endif

# Platform specific configuration
include $(PATH_haussmann)/src/configs/$(PLATFORM).mak
