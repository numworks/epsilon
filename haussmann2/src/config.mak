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

# Platform specific configuration
include $(PATH_haussmann)/src/configs/$(PLATFORM).mak
