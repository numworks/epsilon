define _help_string
Invoking make with \033[38;5;34mPLATFORM=$(PLATFORM)\033[0m.

Build a goal by calling
  \033[38;5;20mmake <goal>.<optional flavors>.<extension>\033[0m

This platform provides the following goals: $(foreach g,$(sort $(ALL_GOALS)),\n\
$(_null) \033[38;5;20m$g\033[0m$(if $(HELP_GOAL_$g),\n    ↳ $(HELP_GOAL_$g),))

This platform provides the following extensions: $(foreach g,$(sort $(ALL_EXTENSIONS)),\n\
$(_null) \033[38;5;20m.$g\033[0m$(if $(HELP_EXTENSION_$g),\n    ↳ $(HELP_EXTENSION_$g),))
endef

export _help_string
.PHONY: help
help:
	@ echo "$$_help_string"

# Display dependencies tree of modules versions
define _modules_string
Goal \033[38;5;34m$*\033[0m uses modules:$(foreach m,$(sort $(call flavorless_modules_for_flavored_goal,$*)),\n\033[38;5;20m$m@$(VERSION_$m)\033[0m
$(if $(findstring :,$(SOURCES_$m)),  flavors: $(call _extract_tastes,$(SOURCES_$m)),) $(foreach n,$(LOCKS_$m),\n\
$(_null) requires: \033[38;5;20m$n@$(VERSION_$n_FOR_$m)\033[0m$(if $(filter $(VERSION_$n),$(VERSION_$n_FOR_$m)),, \033[38;5;9m!= $n@$(VERSION_$n)\033[0m)))
endef

export _modules_string
%.modules:
	@ echo "$$_modules_string"

$(call document_extension,modules,List the goal's modules flavors and dependencies)

# Helpers

define _extract_tastes
$(sort $(subst :,,$(filter :%,$(subst :-, :,$(subst :+, :,$1)))))
endef
