define HELP_STRING
Invoking make with \033[38;5;34mPLATFORM=$(PLATFORM)\033[0m.

Build a goal by calling
  make <goal>.<optional flavors>.<extension>

This platform provides the following goals: $(foreach g,$(ALL_GOALS),\n\
$(_null) $g$(if $(HELP_GOAL_$g),\n    ↳ $(HELP_GOAL_$g),))

This platform provides the following extensions: $(foreach g,$(ALL_EXTENSIONS),\n\
$(_null) $g$(if $(HELP_EXTENSION_$g),\n    ↳ $(HELP_EXTENSION_$g),))
endef
