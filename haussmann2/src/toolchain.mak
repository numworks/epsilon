# Compiler commands and parameters

PYTHON := $(if $(shell $(call folder_check,.venv)),python3,.venv/bin/python3)

HOSTCC := gcc
HOSTCXX := g++
HOSTLD := g++
HOSTCPP := cpp # The C preprocessor, nothing to do with C++

SFLAGS := \
  -DDEBUG=$(DEBUG) \
  -I. \
  -I$(OUTPUT_DIRECTORY) \
  -MMD \
  -MP \
  -Wall

ifeq ($(DEBUG),0)
SFLAGS += -Os -DNDEBUG
else
SFLAGS += -O0 -g
endif

CFLAGS := -std=c11

CXXFLAGS := \
  -ffp-contract=off \
  -fno-exceptions \
  -fno-threadsafe-statics \
  -std=c++20

ifneq ($(_cxx_rtti),1)
CXXFLAGS += -fno-rtti
endif

ARFLAGS := rcs

LDFLAGS :=

include $(PATH_haussmann)/src/toolchains/$(TOOLCHAIN).mak
