# TODO documentation

define declare_goal
$(call _assert_valid_goal_name,$1)
MODULES_$1 := $2

$(OUTPUT_DIRECTORY)/$1%$(EXECUTABLE_EXTENSION): SFLAGS += $$(foreach m,$2,$$(SFLAGS_$$(call name_for_flavored_target,$$m)))
$(OUTPUT_DIRECTORY)/$1%$(EXECUTABLE_EXTENSION): LDFLAGS += $$(foreach m,$2,$$(LDFLAGS_$$(call name_for_flavored_target,$$m)))

$1%: $(OUTPUT_DIRECTORY)/$1%
	@ :

endef

# Do not use flavors_for_flavored_target to avoid an extraneous subst.
define libraries_for_flavored_goal
$(addprefix $(OUTPUT_DIRECTORY)/,\
	$(addsuffix $(subst $( ),,$(filter .%,$(subst ., .,$1))).a,\
	$(MODULES_$(call name_for_flavored_target,$1))))
endef

# Helpers

# _assert_valid_goal_name, <name>
define _assert_valid_goal_name
$(if $(shell [[ "$1" =~ [^a-z0-9_] ]] && echo error),\
	$(error "Error: goal name should only contain lowercase letters, digits, and underscores"),)
endef
