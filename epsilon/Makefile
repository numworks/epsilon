# Import haussmann

PATH_haussmann := ../haussmann
APP_NAME := Epsilon
APP_VERSION := 24.0.0
OUTPUT_ROOT := output
DEBUG ?= 0
PLATFORM ?= host

ALL_SPECIAL_SUBDIRECTORIES := bootloader kernel coverage

GCC_MINIMUM_VERSION := 11
CLANG_MINIMUM_VERSION := 14

ION_layout_variant := epsilon
include $(PATH_haussmann)/Makefile

# Further configuration

EXTERNAL_APPS_API_LEVEL ?= 0

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS) \
  -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

include build/config.$(PLATFORM_TYPE).mak

QUIZ_cases_directories := \
  apps \
  escher \
  python \
  ../poincare \
  ../shared/ion \
  ../shared/kandinsky \
  ../shared/liba \
  ../shared/libaxx \
  ../shared/omg \

# Select the font and layout
KANDINSKY_font_variant := epsilon
ION_layout_variant := epsilon

POINCARE_variant := epsilon

# Import modules

$(call import_module,liba,../shared/liba)
$(call import_module,libaxx,../shared/libaxx)
$(call import_module,omg,../shared/omg)
$(call import_module,kandinsky,../shared/kandinsky)
$(call import_module,ion,../shared/ion)
$(call import_module,quiz,../shared/quiz)
$(call import_module,poincare,../poincare)
$(call import_module,eadk,eadk)
$(call import_module,escher,escher)
$(call import_module,python,python)
$(call import_module,apps,apps)

# Declare goals

include build/rules.$(PLATFORM_TYPE).mak
