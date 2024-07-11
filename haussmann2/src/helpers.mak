# Short helper functions

# Define variables to help manipulate special characters
_null :=
_space := $(_null) $(_null)
$(_space) := $(_space)
, := ,

# text_or, <text>, <fallback if empty>
define text_or
$(if $1,$1,$2)
endef

# capitalize, <text>
# aBcDe -> Abcde
define capitalize
$(shell echo $1 | awk '{print toupper(substr($$0,1,1)) tolower(substr($$0,2))}')
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
$(eval \
ALL_EXTENSIONS += $1
HELP_EXTENSION_$1 := $2
)
endef

# document_other_target, <name>, <documentation>
define document_other_target
$(eval \
ALL_OTHER_TARGETS += $1
HELP_OTHER_TARGET_$1 := $2
)
endef

# Errors out if the path is absolute, or if it contains returns to parent
# directory (e.g. a/../b). This is used in make clean to forbid removing a
# directory out of the building tree.
# assert_relative_descendant, <path>
define assert_relative_descendant
$(if $(filter /% ~/%,$1/.)$(findstring ..,$1/.),$(error "Error: the path cannot contain .. and must be relative"),)
endef

# all_targets_named, <target stem>
# if no arch is defined: stem -> $(OUTPUT_DIRECTORY)/stem
# if archs are defined: stem -> $(OUTPUT_DIRECTORY)/arch1/stem $(OUTPUT_DIRECTORY)/arch2/stem
define all_targets_named
$(strip $(addprefix $(OUTPUT_DIRECTORY)/,\
	$(if $(ARCHS),$(addsuffix /$1,$(ARCHS)),$1)\
))
endef

# all_objects_for, <sources list>
define all_objects_for
$(strip $(if $(ARCHS),\
	$(foreach a,$(ARCHS),$(call objects_for_sources,$a/,$1) $(foreach d,$(ALL_SPECIAL_SUBDIRECTORIES),$(call objects_for_sources,$a/$d/,$1))),\
	$(call objects_for_sources,,$1) $(foreach d,$(ALL_SPECIAL_SUBDIRECTORIES),$(call objects_for_sources,$d/,$1))))
endef

# strip_directory, <directories>, <path>
define strip_directory
$(call text_or,$(strip $(foreach d,$1,$(patsubst $d/%,%,$(filter $d/%,$2)))),$2)
endef

# strip_arch_and_special_dir, <path>
define strip_arch_and_special_dir
$(call strip_directory,$(ALL_SPECIAL_SUBDIRECTORIES),$(call strip_directory,$(ARCHS),$1))
endef

# rule_label, <label>
define rule_label
@ echo "$(shell printf "%-8s" $(strip $(1)))$(@:$(OUTPUT_DIRECTORY)/%=%)"
endef

# simple_rule, <label>, <source extension>, <command>
define rule_for_object
$(eval \
$(OUTPUT_DIRECTORY)/%.o: $$$$(call strip_arch_and_special_dir,%).$(strip $2) | $$$$(@D)/.
	$$(call rule_label,$1)
	$3

$(OUTPUT_DIRECTORY)/%.o: $(OUTPUT_DIRECTORY)/$$$$(call strip_arch_and_special_dir,%).$(strip $2) | $$$$(@D)/.
	$$(call rule_label,$1)
	$3
)
endef
