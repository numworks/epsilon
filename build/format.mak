CXXFORMAT := clang-format
CXXFORMATARGS ?= -i --Werror --verbose

.PHONY: format
format: BASE ?= HEAD
format: FILES ?= $(shell (git diff --name-only $(BASE); git diff --name-only --staged; git ls-files --others --exclude-standard) | grep --extended-regexp "(\.cpp|\.h)")
format:
# Use xargs to elegantly handle the case FILES=""
	$(Q) echo $(FILES) | xargs $(CXXFORMAT) $(CXXFORMATARGS)
