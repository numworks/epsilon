# Provide functions to create goals.
#
# Goals are the main entrypoints into the build sytem. They are defined as the
# list of modules used to build the executable.
# Each module in the goal declaration can be followed by a dot-separated list of
# flavors. Flavors added this way will always be sent to the relevant module
# (and only this one) no matter what flavors are passed to the goal.
#
# Applications use the following function to create a goal:
#   create_goal, <name>, <modules>, <optional description>
# This will create a variable MODULE_<name> holding the list of modules, along
# with a short target allowing a user to build the goal without having to type
# the full path (e.g. goal.a.bin instead of out/put/dir/ect/ory/goal.a.bin).
# The description will be displayed in the make help message.

# Public API

# create_goal, <name>, <modules>, <optional subdirectory>, <optional description>
# FIXME Because of the way target-specific variables work, we cannot easily
# propagate dynamic flavors to the sflags, so only static flavors written in
# create_goal are taken into account.
# e.g. If goal requires module.f1, then when building goal.f2.ext and thus
# module.f1.f2.a, only f1 will be used to filter the sflags.
define create_goal
$(eval \
$(call _assert_valid_goal_name,$1)
ALL_GOALS += $1
$(if $(filter-out $(ALL_SPECIAL_SUBDIRECTORIES),$3),$(error $3 should have been added to ALL_SPECIAL_SUBDIRECTORIES prior to including haussmann),)
MODULES_$1 := $2
HELP_GOAL_$1 := $4

$(call all_targets_named,$(if $3,$3/,)$1%$(EXECUTABLE_EXTENSION)): GOAL := $1
$(call all_targets_named,$(if $3,$3/,)$1%$(EXECUTABLE_EXTENSION)): SFLAGS += $$(foreach m,$2,$$(call sflags_for_flavored_module,$$m))

$$(call all_objects_for,$$(call all_potential_sources,$1)): $$(foreach m,$2,$$(call priority_targets_for_flavored_module,$$m))

$1%: $(call all_targets_named,$(if $3,$3/,)$1%)
	@ :
)
endef

# create_tool, <name>, <modules>
define create_tool
$(eval \
MODULES_$1 := $2
$(TOOLS_DIRECTORY)/$1: TOOLS_SFLAGS += $$(foreach m,$2,$$(call sflags_for_flavored_module,$$m))
)
endef

# create_zip, <zip file>, <contents>
define create_zip
$(eval \
$1: $(OUTPUT_DIRECTORY)/$1
	@ :

$(OUTPUT_DIRECTORY)/$1: $2
	$$(call rule_label,ZIP)
	$(QUIET) rm -rf $$(basename $$@) && mkdir -p $$(basename $$@) && cp $$^ $$(basename $$@) && zip -r -9 -j $$@ $$(basename $$@) > /dev/null && rm -rf $$(basename $$@)
)
endef

# Private API

# flavorless_modules_for_flavored_goal, <flavored goal>
define flavorless_modules_for_flavored_goal
$(foreach m,$(MODULES_$(call name_for_flavored_target,$1)),$(call name_for_flavored_target,$m))
endef

# libraries_for_flavored_goal, <flavored goal>
# $1 might contain an arch, hence the $(dir $1).
# Do not use flavors_for_flavored_target to avoid an extraneous subst.
# If the argument does not match any goal, return _UNREACHABLE instead of the
# empty string. This is to avoid a non-goal target latching onto an implicit
# rule when libraries_for_flavored_goal is used to generate the prerequisites.
define libraries_for_flavored_goal
$(call text_or,$(addprefix $(OUTPUT_DIRECTORY)/$(subst ./,,$(dir $1)),\
	$(addsuffix $(subst $( ),,$(filter .%,$(subst ., .,$(notdir $1)))).a,\
	$(MODULES_$(call name_for_flavored_target,$1)))), \
	_UNREACHABLE)
endef

# ldflags_for_flavored_goal, <flavored goal>
define ldflags_for_flavored_goal
$(LDFLAGS) $(foreach l,$(call libraries_for_flavored_goal,$1),$(call ldflags_for_flavored_module,$(patsubst $(OUTPUT_DIRECTORY)/%.a,%,$l)))
endef

# lddeps_for_flavored_goal, <flavored goal>
define lddeps_for_flavored_goal
$(foreach l,$(call libraries_for_flavored_goal,$1),$(call lddeps_for_flavored_module,$(patsubst $(OUTPUT_DIRECTORY)/%.a,%,$l)))
endef

# all_potential_sources, <goal>
# Return any source file that appears in the goal's modules, regardless of
# tastes.
define all_potential_sources
$(foreach m,$(call flavorless_modules_for_flavored_goal,$1),$(call tasteless_filter,$(SOURCES_$m)))
endef

# Helpers

# _assert_valid_goal_name, <name>
define _assert_valid_goal_name
$(if $(shell [[ "$1" =~ [^a-z0-9_] ]] && echo error),\
	$(error "Error: goal name should only contain lowercase letters, digits, and underscores"),)
endef
