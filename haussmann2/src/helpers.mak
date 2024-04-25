# TODO documentation

# Define variables to help manipulate special characters
_null :=
_space := $(_null) $(_null)
$(_space) := $(_space)

# name_for_flavored_target, <flavored target>
#   name.flavor1.flavor2 -> name
define name_for_flavored_target
$(firstword $(subst ., ,$1))
endef

# flavors_for_flavored_target, <flavored target>
#   name.flavor1.flavor2 -> flavor1 flavor2
define flavors_for_flavored_target
$(subst .,,$(filter .%,$(subst ., .,$1)))
endef

define objects_for_sources
$(addprefix $(OUTPUT_DIRECTORY)/,$(addsuffix .o,$(basename $1)))
endef
