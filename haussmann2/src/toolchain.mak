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

ifeq ($(ASAN),1)
SFLAGS += -fsanitize=address -DASAN
LDFLAGS += -fsanitize=address
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


# Only the major version is checked, the parameter is expected to be an integer
# check_compiler_version,<minimum_required_version>
define check_compiler_version
$(if $(shell if [ $(word 1,$(subst ., ,$(shell $(CC) -dumpversion))) -lt $1 ]; then \
	           echo error; \
	           fi),\
	$(error "Failed requirement, $(CC) must have a version >= $1 but has version $(shell $(CC) -dumpversion)"),)
endef

ifeq ($(lastword $(subst /, ,$(CC))),gcc)
ifneq ($(GCC_MINIMUM_VERSION),)
$(call check_compiler_version,$(GCC_MINIMUM_VERSION))
endif
endif

ifeq ($(lastword $(subst /, ,$(CC))),clang)
ifneq ($(CLANG_MINIMUM_VERSION),)
$(call check_compiler_version,$(CLANG_MINIMUM_VERSION))
endif
endif
