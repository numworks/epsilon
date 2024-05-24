# Compiler commands and parameters

SFLAGS := \
  -I. \
  -MMD \
  -MP \
  -Wall

ifeq ($(DEBUG),0)
SFLAGS += -Os
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
