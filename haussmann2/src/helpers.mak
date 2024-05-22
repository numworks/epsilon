# Short helper functions

# Define variables to help manipulate special characters
_null :=
_space := $(_null) $(_null)
$(_space) := $(_space)

# text_or, <text>, <fallback if empty>
define text_or
$(if $1,$1,$2)
endef

# name_for_flavored_target, <flavored target>
#   name.flavor1.flavor2 -> name
define name_for_flavored_target
$(firstword $(subst ., ,$(notdir $1)))
endef

# flavors_for_flavored_target, <flavored target>
#   name.flavor1.flavor2 -> flavor1 flavor2
define flavors_for_flavored_target
$(subst .,,$(filter .%,$(subst ., .,$1)))
endef

# objects_for_sources, <arch directory>, <sources list>
define objects_for_sources
$(addprefix $(OUTPUT_DIRECTORY)/$1,$(addsuffix .o,$(basename $2)))
endef

# document_extension, <name>, <documentation>
define document_extension
ALL_EXTENSIONS += $1
HELP_EXTENSION_$1 := $2

endef

# Errors out if the path is absolute or contains returns to parent directory.
# This is used in make clean to forbid removing a directory out of the building
# tree.
# assert_relative_descendant, <path>
define assert_relative_descendant
$(if $(filter /% ~/%,$1/.)$(findstring ..,$1/.),$(error "Error: the path cannot contain .. and must be relative"),)
endef

# target_foreach_arch, <target stem>
# if no arch is defined: stem -> $(OUTPUT_DIRECTORY)/stem
# if archs are defined: stem -> $(OUTPUT_DIRECTORY)/arch1/stem $(OUTPUT_DIRECTORY)/arch2/stem
define target_foreach_arch
$(strip $(addprefix $(OUTPUT_DIRECTORY)/,\
	$(if $(ARCHS),$(addsuffix /$1,$(ARCHS)),$1)\
))
endef

# strip_arch_dir, <path>
define strip_arch_dir
$(call text_or,$(strip $(foreach a,$(ARCHS),$(patsubst $a/%,%,$(filter $a/%,$1)))),$1)
endef

# rule_label, <label>
define rule_label
@ echo "$(shell printf "%-8s" $(strip $(1)))$(@:$(OUTPUT_DIRECTORY)/%=%)"
endef
