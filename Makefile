# Import haussmann

PATH_haussmann := haussmann
APP_NAME := Epsilon
APP_VERSION := 23.1.0
OUTPUT_ROOT := output
DEBUG ?= 0
PLATFORM ?= n0110
ASSERTIONS ?= $(DEBUG)

ALL_SPECIAL_SUBDIRECTORIES := bootloader kernel coverage

include $(PATH_haussmann)/Makefile

GCC_MINIMUM_VERSION:=11
CLANG_MINIMUM_VERSION:=14

# TODO: move this function to haussmann?
# Only the major version is checked, the second parameter is expected to be an integer
# check_minimum_version, <compiler>, <minimum_required_version_id>
define check_minimum_version
	@echo Using $1 $(shell $1 -dumpversion)
	$1 --version
	if [ $(word 1,$(subst ., ,$(shell $1 -dumpversion))) -lt $2 ]; then \
	  echo "Failed requirement, $1 should have a version >= $2"; \
    exit 1; \
	fi
endef

.PHONY: check_compiler_version
ifeq ($(lastword $(subst /, ,$(CC))),gcc)
check_compiler_version:
	$(call check_minimum_version,gcc,$(GCC_MINIMUM_VERSION))
endif
ifeq ($(lastword $(subst /, ,$(CC))),clang)
check_compiler_version:
	$(call check_minimum_version,clang,$(CLANG_MINIMUM_VERSION))
endif

# Further configuration

EXTERNAL_APPS_API_LEVEL ?= 0

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS) \
  -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

include build/config.$(PLATFORM_TYPE).mak

# Select the font and layout
KANDINSKY_font_variant := epsilon
ION_layout_variant := epsilon

# Import modules

$(call import_module,liba,liba)
$(call import_module,libaxx,libaxx)
$(call import_module,omg,omg)
$(call import_module,kandinsky,kandinsky)
$(call import_module,ion,ion)
$(call import_module,eadk,eadk)
$(call import_module,poincare,poincare)
$(call import_module,escher,escher)
$(call import_module,python,python)
$(call import_module,apps,apps)
$(call import_module,quiz,quiz)

# Declare goals

include build/rules.$(PLATFORM_TYPE).mak
