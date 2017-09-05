# You can override those settings on the command line

PLATFORM ?= device
VERBOSE ?= 0
DEBUG ?= 0
APPS_LIST?=numworks.calculation numworks.graph numworks.sequence \
  numworks.settings numworks.statistics numworks.probability \
  numworks.regression numworks.code

# Do not edit below this

ifeq ($(DEBUG),1)
OPTIM_SFLAGS ?= -O0 -g
else
OPTIM_SFLAGS ?= -Os
endif

include build/platform.$(PLATFORM).mak
include build/toolchain.$(TOOLCHAIN).mak

OS_WITH_ONBOARDING_APP ?= 1
OS_WITH_SOFTWARE_UPDATE_PROMPT ?= 1
QUIZ_USE_CONSOLE ?= 0

SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DOS_WITH_ONBOARDING_APP=$(OS_WITH_ONBOARDING_APP)
SFLAGS += -DOS_WITH_SOFTWARE_UPDATE_PROMPT=$(OS_WITH_SOFTWARE_UPDATE_PROMPT)
SFLAGS += -DQUIZ_USE_CONSOLE=$(QUIZ_USE_CONSOLE)
SFLAGS += -DAPPS_LIST='"$(APPS_LIST)"'

