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

$(eval $(call import_module,liba,liba))
$(eval $(call import_module,libaxx,libaxx))
$(eval $(call import_module,omg,omg))
$(eval $(call import_module,kandinsky,kandinsky))
$(eval $(call import_module,sdl,ion/src/simulator/external))
$(eval $(call import_module,ion,ion))
$(eval $(call import_module,poincare,poincare))

# FIXME
$(eval $(call import_module,dummy,dummy))

# Declare goals

$(eval $(call create_goal,device, \
  liba \
  libaxx \
  omg \
  kandinsky \
))

$(eval $(call create_goal,simulator, \
  dummy.ion \
  ion \
  kandinsky \
  omg \
  poincare \
  sdl \
))
