# You can override those settings on the command line

PLATFORM ?= device
DEBUG ?= 0
LEDS_CHOICE ?= 0

include build/defaults.mak
include build/platform.$(PLATFORM).mak

EPSILON_VERSION ?= 13.0.0
EPSILON_CUSTOM_VERSION ?= 1.19.0-0
# USERNAME ?= N/A
EPSILON_APPS ?= calculation rpn graph code statistics probability solver atom sequence regression settings external
EPSILON_I18N ?= en fr es de pt hu
# EPSILON_I18N ?= en fr es de pt hu
EPSILON_GETOPT ?= 0
EPSILON_TELEMETRY ?= 0
ESCHER_LOG_EVENTS_BINARY ?= 0
OMEGA_THEME ?= omega_light

ifndef USE_LIBA
  $(error platform.mak should define USE_LIBA)
endif
ifndef EXE
  $(error platform.mak should define EXE, the extension for executables)
endif
include build/toolchain.$(TOOLCHAIN).mak

SFLAGS += -DDEBUG=$(DEBUG)
SFLAGS += -DLEDS_CHOICE=$(LEDS_CHOICE)
ifdef USERNAME
  SFLAGS += -DUSERNAME="$(USERNAME)"
endif
SFLAGS += -DEPSILON_GETOPT=$(EPSILON_GETOPT)
SFLAGS += -DEPSILON_TELEMETRY=$(EPSILON_TELEMETRY)
SFLAGS += -DESCHER_LOG_EVENTS_BINARY=$(ESCHER_LOG_EVENTS_BINARY)
