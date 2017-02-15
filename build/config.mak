# You can edit this file to change build settings

PLATFORM ?= device
VERBOSE ?= 0
DEBUG ?= 1
LIBA_LOG_DYNAMIC_MEMORY ?= 0
ESCHER_LOG_EVENTS ?= 0
ION_EVENTS ?= keyboard
# Possible values : keyboard, stdin, random, replay

# Do not edit below this

ifeq ($(DEBUG),1)
OPTIM_SFLAGS ?= -O0
else
OPTIM_SFLAGS ?= -Os
endif

include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak
