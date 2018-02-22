# You can override those settings on the command line

PLATFORM ?= device
DEBUG ?= 0

EPSILON_VERSION ?= 1.3.0
EPSILON_ONBOARDING_APP ?= 1
EPSILON_SOFTWARE_UPDATE_PROMPT ?= 1
EPSILON_APPS ?= calculation graph sequence settings statistics probability regression code
EPSILON_I18N ?= en fr es de pt
EPSILON_GETOPT ?= 0

include build/defaults.mak
include build/platform.$(PLATFORM).mak
ifndef USE_LIBA
  $(error platform.mak should define USE_LIBA)
endif
ifndef EXE
  $(error platform.mak should define EXE, the extension for executables)
endif
include build/toolchain.$(TOOLCHAIN).mak

SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DEPSILON_ONBOARDING_APP=$(EPSILON_ONBOARDING_APP)
SFLAGS += -DEPSILON_SOFTWARE_UPDATE_PROMPT=$(EPSILON_SOFTWARE_UPDATE_PROMPT)
SFLAGS += -DEPSILON_GETOPT=$(EPSILON_GETOPT)
