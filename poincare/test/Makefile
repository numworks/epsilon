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

# Modules import
$(call import_module,omg,../../shared/omg)

KANDINSKY_font_variant := epsilon
$(call import_module,kandinsky,../../shared/kandinsky)

ION_variant := epsilon
ION_external_apps := 0
$(call import_module,ion,../../shared/ion)
$(call import_module,sdl,$(PATH_ion)/src/simulator/external)

POINCARE_variant := epsilon
$(call import_module,poincare,../../poincare)

$(call import_module,quiz,../../shared/quiz)

# Goal definition
$(call create_goal,poincare_test, \
  omg \
  kandinsky \
  ion \
  sdl \
  poincare.test \
  quiz.arguments \
)
