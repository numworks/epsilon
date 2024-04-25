# TODO documentation

define define_module
$(call _assert_valid_module_name,$1)
PATH_$1 ?= .
SOURCES_$1 = $(addprefix $$(PATH_$1)/,$(strip $2))
SFLAGS_$1 = -I$$(PATH_$1)/include

endef

define declare_module
$(call _assert_valid_module_name,$1)
PATH_$1 := $2
-include $2/Makefile

endef

# Helpers

# _assert_valid_module_name, <name>
define _assert_valid_module_name
$(if $(shell [[ "$1" =~ [^a-z0-9_] ]] && echo error),\
	$(error "Error: module name should only contain lowercase letters, digits, and underscores"),)
endef
