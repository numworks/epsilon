# You can override those settings on the command line

PLATFORM ?= device
DEBUG ?= 0

include build/defaults.mak
include build/platform.$(PLATFORM).mak

EPSILON_VERSION ?= 13.0.0
EPSILON_APPS ?= calculation graph code statistics probability solver sequence regression settings
EPSILON_I18N ?= en fr es de pt
EPSILON_GETOPT ?= 0
EPSILON_TELEMETRY ?= 0
ESCHER_LOG_EVENTS_BINARY ?= 0

ifndef USE_LIBA
  $(error platform.mak should define USE_LIBA)
endif
ifndef EXE
  $(error platform.mak should define EXE, the extension for executables)
endif
include build/toolchain.$(TOOLCHAIN).mak

SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DEPSILON_GETOPT=$(EPSILON_GETOPT)
SFLAGS += -DEPSILON_TELEMETRY=$(EPSILON_TELEMETRY)
SFLAGS += -DESCHER_LOG_EVENTS_BINARY=$(ESCHER_LOG_EVENTS_BINARY)
