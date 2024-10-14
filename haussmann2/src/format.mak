CXXFORMAT := clang-format
CXXFORMATARGS ?= -i --Werror --verbose
GITDIFFARGS := --name-only --diff-filter=d
PYFORMAT := ./.venv/bin/black
PYFORMATARGS ?=
PRETTIER := npx prettier
PRETTIERARGS ?= --write

.PHONY: format
format: BASE ?= HEAD
format: CXXFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard) | grep --extended-regexp "(\.cpp$$|\.h$$|libaxx/include)")
format: PYFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard) | grep --extended-regexp "(\.py$$)")
format: JSFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard) | grep --extended-regexp "(\.js$$|\.cjs$$|\.mjs$$|\.ts$$)")
format:
# Use xargs to elegantly handle the case CXXFILES=""
	echo "=== Formatting .cpp and .h files ==="
	echo $(CXXFILES) | xargs -r $(CXXFORMAT) $(CXXFORMATARGS)
	echo "=== Formatting .py files ==="
	echo $(PYFILES) | xargs -r $(PYFORMAT) $(PYFORMATARGS)
	echo "=== Formatting .js files ==="
	echo $(JSFILES) | xargs -r $(PRETTIER) $(PRETTIERARGS)

.PHONY: reformat
reformat:
	@if [ "$(BASE)" == "" ]; then \
	  echo "Please provide a base commit with \"make reformat BASE=<ref>\""; \
	else \
	  git rebase --autostash --strategy-option=theirs --exec "make format BASE=HEAD~; git commit -a --amend --no-edit" $(BASE); \
	fi

$(call document_other_target,format,Format C++ and Python files. Apply on files specified explicitly (in CXXFILES and PYFILES variables) or on files modified since BASE (defaults to HEAD))
$(call document_other_target,reformat,Rebase and apply make format to all commits since BASE)
