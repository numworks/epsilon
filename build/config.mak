# You can override those settings on the command line

PLATFORM ?= device
VERBOSE ?= 0
DEBUG ?= 1

# Do not edit below this

ifeq ($(DEBUG),1)
OPTIM_SFLAGS ?= -O0
else
OPTIM_SFLAGS ?= -Os
endif

include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak
