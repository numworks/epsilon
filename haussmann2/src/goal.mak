# Provide functions to create goals.
#
# Goals are the main entrypoints into the build sytem. They are defined as the
# list of modules used to build the executable.
# Each module in the goal declaration can be followed by a dot-separated list of
# flavors. Flavors added this way will always be sent to the relevant module
# (and only this one) no matter what flavors are passed to the goal.
#
# Applications use the following function to create a goal:
#   create_goal, <name>, <modules>
# This will create a variable MODULE_<name> holding the list of modules, along
# with a short target allowing a user to build the goal without having to type
# the full path (e.g. goal.a.bin instead of out/put/dir/ect/ory/goal.a.bin).

# Public API

# create_goal, <name>, <modules>
define create_goal
$(call _assert_valid_goal_name,$1)
ALL_GOALS += $1
MODULES_$1 := $2

$(OUTPUT_DIRECTORY)/$1%$(EXECUTABLE_EXTENSION): SFLAGS += $$(foreach m,$2,$$(SFLAGS_$$(call name_for_flavored_target,$$m)))
$(OUTPUT_DIRECTORY)/$1%$(EXECUTABLE_EXTENSION): LDFLAGS += $$(foreach m,$2,$$(LDFLAGS_$$(call name_for_flavored_target,$$m)))

$1%: $(OUTPUT_DIRECTORY)/$1%
	@ :

endef

# Private API

# libraries_for_flavored_goal, <flavored goal>
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
