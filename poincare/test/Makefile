# Haussmann configuration
PATH_haussmann := ../../haussmann
APP_NAME := PoincareTest
APP_VERSION := 0.0.0
OUTPUT_ROOT := output
DEBUG ?= 1
PLATFORM ?= host
STRICT_WARNINGS ?= 0

include $(PATH_haussmann)/Makefile

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS) \
  -DEXTERNAL_APPS_API_LEVEL=0

ifeq ($(PLATFORM_TYPE),device)
SFLAGS += -DPLATFORM_DEVICE
endif

# Modules import
## OMG
$(call import_module,omg,../../shared/omg)
_poincare_test_modules := omg

## Kandinsky
KANDINSKY_font_variant := epsilon
$(call import_module,kandinsky,../../shared/kandinsky)
_poincare_test_modules += kandinsky

## Ion
ION_variant := epsilon
ION_external_apps := 0
EMBED_EXTRA_DATA := 0
$(call import_module,ion,../../shared/ion)
ifeq ($(PLATFORM_TYPE),simulator)
_poincare_test_modules += ion
else
_poincare_test_modules += ion.userland.consoledisplay.test
endif

## SDL
ifeq ($(PLATFORM_TYPE),simulator)
$(call import_module,sdl,$(PATH_ion)/src/simulator/external)
_poincare_test_modules += sdl
endif

## liba/libaxx
ifeq ($(PLATFORM_TYPE),device)
$(call import_module,liba,../../shared/liba)
$(call import_module,libaxx,../../shared/libaxx)
_poincare_test_modules += liba.armv7m.aeabirt.openbsd libaxx
endif

## Poincare
POINCARE_variant := epsilon
$(call import_module,poincare,../../poincare)
_poincare_test_modules += poincare.test

## Quiz
$(call import_module,quiz,../../shared/quiz)
ifeq ($(PLATFORM_TYPE),simulator)
_poincare_test_modules += quiz.exception.arguments
else
_poincare_test_modules += quiz.exception
endif

# Goal definition
$(call create_goal,poincare_test, \
  $(_poincare_test_modules) \
)
