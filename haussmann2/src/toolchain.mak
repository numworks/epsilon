# Compiler commands and parameters

PYTHON := $(if $(shell test -d '.venv' || echo 1),python3,.venv/bin/python3)

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

ifeq ($(ASSERTIONS),0)
SFLAGS += -DNDEBUG
endif

ifeq ($(DEBUG),0)
SFLAGS += -Os
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

# Set CCACHE=1 in your make command if you want to use ccache to fasten builds
CCACHE ?= 0
ifeq ($(CCACHE),1)
CC := ccache $(CC)
CXX := ccache $(CXX)
AR := ccache $(AR)
endif
