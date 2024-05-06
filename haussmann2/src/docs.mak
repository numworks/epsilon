define _help_string
Invoking make with \033[38;5;34mPLATFORM=$(PLATFORM)\033[0m.

Build a goal by calling
  \033[38;5;20mmake <goal>.<optional flavors>.<extension>\033[0m

This platform provides the following goals: $(foreach g,$(ALL_GOALS),\n\
$(_null) \033[38;5;20m$g\033[0m$(if $(HELP_GOAL_$g),\n    ↳ $(HELP_GOAL_$g),))

This platform provides the following extensions: $(foreach g,$(ALL_EXTENSIONS),\n\
$(_null) \033[38;5;20m.$g\033[0m$(if $(HELP_EXTENSION_$g),\n    ↳ $(HELP_EXTENSION_$g),))
endef

export _help_string
.PHONY: help
help:
	@ echo "$$_help_string"

# Display dependencies tree of modules versions
define _versions_string
Goal $* uses modules:$(foreach m,$(MODULES_$(call name_for_flavored_target,$*)),\n$(call name_for_flavored_target,$m)@$(VERSION_$(call name_for_flavored_target,$m)) $(foreach n,$(LOCKS_$(call name_for_flavored_target,$m)),\n\
$(_null) requires $n@$(VERSION_$n_FOR_$(call name_for_flavored_target,$m))))
endef

export _versions_string
%.versions:
	@ echo "$$_versions_string"

$(eval $(call document_extension,versions,List the goal's modules dependencies))
