define _help_string
Building \033[38;5;2m$(APP_NAME)@$(APP_VERSION)\033[0m with:\033[38;5;2m
  DEBUG=$(DEBUG)
  PLATFORM=$(PLATFORM)
\033[0m
Build a goal by calling
  \033[38;5;20mmake <goal>.<optional flavors>.<extension>\033[0m

This platform provides the following goals: $(foreach g,$(sort $(ALL_GOALS)),\n\
$(_null) \033[38;5;20m$g\033[0m$(if $(HELP_GOAL_$g),\n    ↳ $(HELP_GOAL_$g),))

This platform provides the following extensions: $(foreach g,$(sort $(ALL_EXTENSIONS)),\n\
$(_null) \033[38;5;20m.$g\033[0m$(if $(HELP_EXTENSION_$g),\n    ↳ $(HELP_EXTENSION_$g),))

This platform provides the following additional targets: $(foreach g,$(sort $(ALL_OTHER_TARGETS)),\n\
$(_null) \033[38;5;20m$g\033[0m$(if $(HELP_OTHER_TARGET_$g),\n    ↳ $(HELP_OTHER_TARGET_$g),))
endef

export _help_string
.PHONY: help
help:
	@ echo "$$_help_string"

# Display modules and flavors
define _modules_string
Goal \033[38;5;34m$*\033[0m uses modules:$(foreach m,$(sort $(call flavorless_modules_for_flavored_goal,$*)),\n\033[38;5;20m$m@$(VERSION_$m)\033[0m
$(if $(findstring :,$(SOURCES_$m)),  flavors: $(call _extract_tastes,$(SOURCES_$m)),))
endef

export _modules_string
%.modules:
	@ echo "$$_modules_string"

$(call document_extension,modules,List the goal's modules and their flavors)

$(call document_other_target,help)

# Helpers

define _extract_tastes
$(sort $(subst :,,$(filter :%,$(subst :-, :,$(subst :+, :,$1)))))
endef
