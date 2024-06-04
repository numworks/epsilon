# Import haussmann

PATH_haussmann := haussmann
APP_NAME := Epsilon
APP_VERSION := 23.1.0
OUTPUT_ROOT := output
DEBUG ?= 1
PLATFORM ?= macos
include $(PATH_haussmann)/Makefile

# Further configuration

ASSERTIONS ?= $(DEBUG)
EXTERNAL_APPS_API_LEVEL ?= 0

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS) \
  -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

# Import modules

$(call import_module,liba,liba)
$(call import_module,libaxx,libaxx)
$(call import_module,omg,omg)
$(call import_module,kandinsky,kandinsky)
$(call import_module,sdl,ion/src/simulator/external)
$(call import_module,ion,ion)
$(call import_module,poincare,poincare)
$(call import_module,escher,escher)
$(call import_module,apps,apps)

# FIXME
$(call import_module,dummy,dummy)

# Declare goals

$(call create_goal,device, \
  liba \
  libaxx \
  omg \
  kandinsky \
)

$(call create_goal,simulator, \
  apps \
  dummy.ion \
  escher \
  ion \
  kandinsky \
  omg \
  poincare \
  sdl \
)
