define HELP_STRING
Invoking make with \033[38;5;34mPLATFORM=$(PLATFORM)\033[0m.
This platform provides the following goals: $(foreach g,$(ALL_GOALS),\n\
$(_null) $g$(if $(HELP_$g),\n    ↳ $(HELP_$g),))
endef
