PATH_haussmann := haussmann
APP_NAME := Epsilon
APP_VERSION := 23.1.0
OUTPUT_ROOT := output
DEBUG ?= 0
PLATFORM ?= n0110
include $(PATH_haussmann)/Makefile

$(eval $(call import_module,liba,liba))
$(eval $(call import_module,libaxx,libaxx))

$(eval $(call create_goal,testpsilon, \
  liba.aeabirt.armv7m.openbsd \
  libaxx \
))
