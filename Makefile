# Import haussmann

PATH_haussmann := haussmann
APP_NAME := Epsilon
APP_VERSION := 23.1.0
OUTPUT_ROOT := output
DEBUG ?= 0
PLATFORM ?= n0110
ASSERTIONS ?= $(DEBUG)

ALL_SPECIAL_SUBDIRECTORIES := bootloader kernel

include $(PATH_haussmann)/Makefile

# Further configuration

EXTERNAL_APPS_API_LEVEL ?= 0

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS) \
  -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

include build/config.$(PLATFORM_TYPE).mak

# Select the simulator layout
ION_layout_variant := epsilon

# Import modules

$(call import_module,liba,liba)
$(call import_module,libaxx,libaxx)
$(call import_module,omg,omg)
$(call import_module,kandinsky,kandinsky)
$(call import_module,ion,ion)
$(call import_module,poincare,poincare)
$(call import_module,escher,escher)
$(call import_module,python,python)
$(call import_module,apps,apps)
$(call import_module,quiz,quiz)

# Declare goals

include build/rules.$(PLATFORM_TYPE).mak
