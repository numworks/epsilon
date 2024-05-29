PATH_haussmann := haussmann
APP_NAME := Epsilon
APP_VERSION := 23.1.0
OUTPUT_ROOT := output
DEBUG ?= 1
PLATFORM ?= macos
include $(PATH_haussmann)/Makefile

$(eval $(call import_module,liba,liba))
$(eval $(call import_module,libaxx,libaxx))
$(eval $(call import_module,omg,omg))
$(eval $(call import_module,kandinsky,kandinsky))
$(eval $(call import_module,sdl,ion/src/simulator/external))

# FIXME
$(eval $(call import_module,dummy,dummy))

$(eval $(call create_goal,device, \
  liba \
  libaxx \
  omg \
  kandinsky \
))

$(eval $(call create_goal,simulator, \
  dummy \
  sdl \
))
