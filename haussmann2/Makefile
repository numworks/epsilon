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

.PHONY: clean
clean:
	$(QUIET) rm -rf $(OUTPUT_DIRECTORY)

include $(PATH_haussmann)/src/docs.mak
export HELP_STRING
.PHONY: help
help:
	@ echo "$$HELP_STRING"
