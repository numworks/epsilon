.SUFFIXES:

.SECONDARY:

.SECONDEXPANSION:

# assert_defined, <variable>
define assert_defined
$(if $($1),,$(error Error: Undefined variable $1))
endef

$(call assert_defined,PATH_haussmann)

include $(PATH_haussmann)/src/config.mak
include $(PATH_haussmann)/src/toolchain.mak

include $(PATH_haussmann)/src/helpers.mak
include $(PATH_haussmann)/src/flavor.mak

include $(PATH_haussmann)/src/goal.mak
include $(PATH_haussmann)/src/module.mak

include $(PATH_haussmann)/src/rules.mak
