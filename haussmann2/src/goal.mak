# TODO documentation

define declare_goal
$(call _assert_valid_goal_name,$1)
MODULES_$1 := $2

$(OUTPUT_DIRECTORY)/$1%$(EXECUTABLE_EXTENSION): SFLAGS += $$(foreach m,$2,$$(SFLAGS_$$m))
$(OUTPUT_DIRECTORY)/$1%$(EXECUTABLE_EXTENSION): LDFLAGS += $$(foreach m,$2,$$(LDFLAGS_$$m))

endef

# Helpers

# _assert_valid_goal_name, <name>
define _assert_valid_goal_name
$(if $(shell [[ "$1" =~ [^a-z0-9_] ]] && echo error),\
	$(error "Error: goal name should only contain lowercase letters, digits, and underscores"),)
endef
