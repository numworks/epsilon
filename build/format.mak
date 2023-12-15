CXXFORMAT := clang-format
CXXFORMATARGS ?= -i --Werror --verbose
GITDIFFARGS := --name-only --diff-filter=d
PYFORMAT := ./.venv/bin/black
PYFORMATARGS ?=

.PHONY: format
format: BASE ?= HEAD
format: CXXFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard) | grep --extended-regexp "(\.cpp$$|\.h$$|libaxx/include)")
format: PYFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard) | grep --extended-regexp "(\.py$$)")
format:
# Use xargs to elegantly handle the case CXXFILES=""
	$(Q) echo "=== Formatting .cpp and .h files ==="
	$(Q) echo $(CXXFILES) | xargs -r $(CXXFORMAT) $(CXXFORMATARGS)
	$(Q) echo "=== Formatting .py files"
	$(Q) echo $(PYFILES) | xargs -r $(PYFORMAT) $(PYFORMATARGS)
