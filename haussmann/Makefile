# Disable built-in rules.
.SUFFIXES:

# Activate second expansion, necessary to use call in prerequisites of pattern
# rules.
.SECONDEXPANSION:

# Never delete intermediate targets.
.SECONDARY:

# assert_defined, <variable>
define assert_defined
$(if $($1),,$(error Error: Undefined variable $1))
endef

# require_make_version, <minimum_version>
define require_make_version
$(if $(filter $1,$(firstword $(sort $(MAKE_VERSION) $1))),,$(error Error: GNU Make >= $1 required (found $(MAKE_VERSION))))
endef

$(call require_make_version,4.3)

$(call assert_defined,PATH_haussmann)

# Default target when calling make without a goal.
.PHONY: default
default: help

# Load the build engine. Order matters.
include $(PATH_haussmann)/src/config.mak
include $(PATH_haussmann)/src/toolchain.mak

include $(PATH_haussmann)/src/helpers.mak
include $(PATH_haussmann)/src/flavor.mak

include $(PATH_haussmann)/src/goal.mak
include $(PATH_haussmann)/src/module.mak

include $(PATH_haussmann)/src/rules.mak

# Miscellaneous rules

%.print_output_directory:
	@echo $(CURDIR)/$(OUTPUT_DIRECTORY)$(if $(ARCHS),/$(ARCHS))

.PHONY: clean
clean:
	$(call assert_relative_descendant,$(OUTPUT_DIRECTORY))
	shopt -s dotglob ; rm -rf $(OUTPUT_DIRECTORY)/*
	rmdir -p $(OUTPUT_DIRECTORY) 2> /dev/null || :

include $(PATH_haussmann)/src/docs.mak
include $(PATH_haussmann)/src/format.mak
include $(PATH_haussmann)/src/tests.mak
