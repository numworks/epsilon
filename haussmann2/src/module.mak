# Provide functions to create and use modules.
#
# Modules are collections of source files with a well defined public API, that
# are compiled to a static library during the build process. They can be
# configured by selecting which files are compiled as part of the library, using
# the flavor syntax.
#
# A module should define these variables in a Makefile at its root:
# - VERSION_<module>: the version of the module
# - SOURCES_<module>: the list of source files for this module, with optional
#   tastes
# - SFLAGS_<module>: compilation flags for users of this module; it should at
#   least contain the -I flag to the module API
# - PRIVATE_SFLAGS_<module>: compilation flags used when compiling the sources
#   this module
# - LDFLAGS_<module>: linker flags for users of this module; for instance,
#   libraries required by the module, or a custom linker script
#
# A module will expect the following variables to be defined prior to its
# inclusion:
# - PATH_<module>: the location of the module in the user application
#
# Inside a module, use:
#   create_module, <name>, <version>, <sources>
# This will create the SOURCES_<...> variable, and a SFLAGS_<...> variable with
# only the -I flag to the module API.
# The files in SOURCES_<...> can be suffixed with tastes (e.g. a.cpp:+b) that
# determine which combination of flavors will cause the file to be compiled.
#
# Inside an application, for each module required, use:
#   import_module, <name>, <path>
# <name> is the identifier of the module, used as a suffix to the module
# variables. <path> is the location of the module inside the application and
# will be used to define PATH_<module>.

# Public API

# create_module, <name>, <version>, <sources>
define create_module
$(eval \
$(call _assert_valid_module_name,$1)
$(call _assert_valid_version,$2)
$(call assert_defined,PATH_$1)
VERSION_$1 := $2
SOURCES_$1 = $(addprefix $$(PATH_$1)/,$(strip $3))
SFLAGS_$1 = -I$$(PATH_$1)/include

$(call target_foreach_arch,$1%a): SFLAGS += $$(PRIVATE_SFLAGS_$1)

$1%a: $(call target_foreach_arch,$1%a)
	@ :
)
endef

# import_module, <name>, <path>
define import_module
$(eval \
$(call _assert_valid_module_name,$1)
PATH_$1 := $2
-include $2/module.mak
-include $2/locks.mak
)
endef

# Private API

# Getters for module variables, using flavors.
# ..._for_flavored_module, <module with dot-separated flavors>
# $1 might be prefixed with an arch's directory.
sources_for_flavored_module = $(call _flavor_filtered_module_variable,$1,SOURCES)
sflags_for_flavored_module = $(call _flavor_filtered_module_variable,$1,SFLAGS)
ldflags_for_flavored_module = $(call _flavor_filtered_module_variable,$1,LDFLAGS)
lddeps_for_flavored_module = $(call _flavor_filtered_module_variable,$1,LDDEPS)
priority_targets_for_flavored_module = $(call _flavor_filtered_module_variable,$1,PRIORITY_TARGETS)

# objects_for_flavored_module, <dot-separated flavored module>
# $1 might be prefixed with an arch's directory.
define objects_for_flavored_module
$(call objects_for_sources,$(subst ./,,$(dir $1)),$(call sources_for_flavored_module,$1))
endef

# Helpers

# _assert_valid_module_name, <name>
define _assert_valid_module_name
$(if $(shell [[ "$1" =~ [^a-z0-9_] ]] && echo error),\
	$(error "Error: module name should only contain lowercase letters, digits, and underscores"),)
endef

define _assert_valid_version
$(if $(shell [[ "$1" =~ ^[0-9]+$$ ]] || echo error),\
	$(error "Error: version should be an integer"),)
endef

# _flavor_filtered_module_variable, <dot-separated flavored module>, <variable name>
define _flavor_filtered_module_variable
$(call flavor_filter,\
	$($2_$(call name_for_flavored_target,$1)),\
	$(call flavors_for_flavored_target,$(notdir $1)))
endef
