HOSTCC = gcc
HOSTCXX = g++
PYTHON = python

# Language-specific flags
CFLAGS = -std=c99
CXXFLAGS = -std=c++11 -fno-exceptions -fno-rtti -fno-threadsafe-statics

# Flags - Optimizations
ifeq ($(DEBUG),1)
SFLAGS = -Og -g
else
SFLAGS = -Os
endif

# Flags - Header search path
SFLAGS += -Ilib -I.

# Flags - Building options
SFLAGS += -Wall

# Flags - Header dependency tracking
SFLAGS += -MD -MP

# Define "Q" as an arobase by default to silence-out every command run by make.
# If V=1 is supplied on the make command line, undefine Q so that every command
# is echoed out.
Q=@
BUILD_VERBOSE=0
ifeq ("$(origin V)", "command line")
  ifeq ($(V),1)
    BUILD_VERBOSE=1
    Q=
  endif
endif

BUILD_DIR = build/$(PLATFORM)

# Define rules for targets
# Those can be built directly with make executable.extension as a shortcut.

define rules_for_targets
.PHONY: $(1).$(2)
$(1).$(2): $$(BUILD_DIR)/$(1).$(2)
endef

