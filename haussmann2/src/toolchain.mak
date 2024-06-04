# Compiler commands and parameters

PYTHON := $(if $(shell $(call folder_check,.venv)),python3,.venv/bin/python3)

HOSTCC := gcc
HOSTCXX := g++
HOSTLD := g++

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
  -fno-rtti \
  -fno-threadsafe-statics \
  -std=c++20

ARFLAGS := rcs

LDFLAGS :=

include $(PATH_haussmann)/src/toolchains/$(TOOLCHAIN).mak
