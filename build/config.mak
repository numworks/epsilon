# You can override those settings on the command line

PLATFORM ?= device
VERBOSE ?= 0
DEBUG ?= 1

# Do not edit below this

ifeq ($(DEBUG),1)
OPTIM_SFLAGS ?= -O0 -g
else
OPTIM_SFLAGS ?= -Os
endif

include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak

OS_WITH_ONBOARDING_APP ?= 1

SFLAGS += -DOS_WITH_ONBOARDING_APP=$(OS_WITH_ONBOARDING_APP)
