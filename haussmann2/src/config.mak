# Configuration variables, may be edited on the command line.

$(call assert_defined,OUTPUT_ROOT)
$(call assert_defined,DEBUG)
$(call assert_defined,PLATFORM)

ifeq ($(DEBUG),0)
_build_type := release
else
_build_type := debug
endif

OUTPUT_DIRECTORY ?= $(OUTPUT_ROOT)/$(_build_type)/$(PLATFORM)

VERBOSE ?= 0
ifeq ($(VERBOSE),0)
QUIET := @
endif

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

# Platform specific configuration
include $(PATH_haussmann)/src/configs/$(PLATFORM).mak
