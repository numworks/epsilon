# You can override those settings on the command line

PLATFORM ?= device
DEBUG ?= 0
EPSILON_VERSION ?= 11.1.0
EPSILON_ONBOARDING_APP ?= 0
# Valid values are "none", "update", "beta"
EPSILON_BOOT_PROMPT ?= none
EPSILON_APPS ?= calculation graph code statistics probability solver sequence regression settings
EPSILON_I18N ?= en fr es de pt
EPSILON_GETOPT ?= 0
MATRICES_ARE_DEFINED ?=1
ESCHER_LOG_EVENTS_BINARY ?= 0

include scripts/defaults.mak
include scripts/platform.$(PLATFORM).mak
ifndef USE_LIBA
  $(error platform.mak should define USE_LIBA)
endif
ifndef EXE
  $(error platform.mak should define EXE, the extension for executables)
endif
include scripts/toolchain.$(TOOLCHAIN).mak

SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DEPSILON_ONBOARDING_APP=$(EPSILON_ONBOARDING_APP)
SFLAGS += -DEPSILON_GETOPT=$(EPSILON_GETOPT)
EPSILON_BETA_PROMPT := 1
EPSILON_UPDATE_PROMPT := 2
SFLAGS += -DEPSILON_BETA_PROMPT=$(EPSILON_BETA_PROMPT)
SFLAGS += -DEPSILON_UPDATE_PROMPT=$(EPSILON_UPDATE_PROMPT)
ifeq (beta,$(EPSILON_BOOT_PROMPT))
SFLAGS += -DEPSILON_BOOT_PROMPT=$(EPSILON_BETA_PROMPT)
endif
ifeq (update,$(EPSILON_BOOT_PROMPT))
SFLAGS += -DEPSILON_BOOT_PROMPT=$(EPSILON_UPDATE_PROMPT)
endif
SFLAGS += -DMATRICES_ARE_DEFINED=$(MATRICES_ARE_DEFINED)
SFLAGS += -DESCHER_LOG_EVENTS_BINARY=$(ESCHER_LOG_EVENTS_BINARY)
