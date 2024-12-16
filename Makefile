# Import haussmann

PATH_haussmann := haussmann
APP_NAME := Epsilon
APP_VERSION := 24.0.0
OUTPUT_ROOT := output
DEBUG ?= 0
PLATFORM ?= n0110

ALL_SPECIAL_SUBDIRECTORIES := bootloader kernel coverage

GCC_MINIMUM_VERSION := 11
CLANG_MINIMUM_VERSION := 14

include $(PATH_haussmann)/Makefile

# Further configuration

EXTERNAL_APPS_API_LEVEL ?= 0

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS) \
  -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

include build/config.$(PLATFORM_TYPE).mak

QUIZ_cases_directories := apps escher ion kandinsky liba libaxx omg poincare python

# Select the font and layout
KANDINSKY_font_variant := epsilon
ION_layout_variant := epsilon

POINCARE_variant := epsilon

# Import modules

$(call import_module,liba,liba)
$(call import_module,libaxx,libaxx)
$(call import_module,omg,omg)
$(call import_module,kandinsky,kandinsky)
$(call import_module,ion,ion)
$(call import_module,eadk,eadk)
$(call import_module,poincare,poincare)
$(call import_module,escher,escher)
$(call import_module,python,python)
$(call import_module,apps,apps)
$(call import_module,quiz,quiz)

# Declare goals

include build/rules.$(PLATFORM_TYPE).mak
