# Compiler commands and parameters

AR := ar
ARFLAGS := rcs

SFLAGS := -MMD -MP -Wall
ifeq ($(DEBUG),0)
SFLAGS += -Os
else
SFLAGS += -O0 -g
endif

CFLAGS := -std=c11
CXXFLAGS := -std=c++20
LDFLAGS :=

include $(PATH_haussmann)/src/toolchain.$(PLATFORM).mak
