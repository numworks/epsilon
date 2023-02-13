CXXFORMAT := clang-format
CXXFORMATARGS ?= -i --Werror --verbose
GITDIFFARGS := --name-only --diff-filter=d

.PHONY: format
format: BASE ?= HEAD
format: FILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard) | grep --extended-regexp "(\.cpp|\.h)")
format:
# Use xargs to elegantly handle the case FILES=""
	$(Q) echo $(FILES) | xargs $(CXXFORMAT) $(CXXFORMATARGS)
