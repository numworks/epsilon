CXXFORMAT := clang-format
CXXFORMATARGS ?= -i --Werror --verbose
GITDIFFARGS := --name-only --diff-filter=d
PYFORMAT := $(PYTHON) -m black
PYFORMATARGS ?=
JSFORMAT := npx prettier
JSFORMATARGS ?= --write
MAKFORMAT := $(PYTHON) $(ROOT)/tools/format_makefiles.py
MAKFORMATARGS ?=
ROOT := .

.PHONY: format
format: BASE ?= HEAD
format: CXXFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard --full-name) | grep --extended-regexp "(\.c$$|\.cpp$$|\.h$$|shared/libaxx/include)")
format: PYFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard --full-name) | grep --extended-regexp "(\.py$$)")
format: JSFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard --full-name) | grep --extended-regexp "(\.js$$|\.cjs$$|\.mjs$$|\.ts$$|\.html)")
format: MAKFILES ?= $(shell (git diff $(GITDIFFARGS) $(BASE); git diff $(GITDIFFARGS) --staged; git ls-files --others --exclude-standard --full-name) | grep --extended-regexp "(Makefile|\.mak)$$")
format:
	@if [ -n "$(CXXFILES)" ]; then \
	  echo "=== Formatting .c, .cpp and .h files ==="; \
	  $(CXXFORMAT) --version; \
	  cd $(ROOT) && echo $(CXXFILES) | xargs -r $(CXXFORMAT) $(CXXFORMATARGS); \
	fi
	@if [ -n "$(PYFILES)" ]; then \
	  echo "=== Formatting .py files ==="; \
	  $(PYFORMAT) --version; \
	  cd $(ROOT) && echo $(PYFILES) | xargs -r $(PYFORMAT) $(PYFORMATARGS); \
	fi
	@if [ -n "$(JSFILES)" ]; then \
	  echo "=== Formatting .js files ==="; \
	  $(JSFORMAT) --version; \
	  cd $(ROOT) && echo $(JSFILES) | xargs -r $(JSFORMAT) $(JSFORMATARGS); \
	fi
	@if [ -n "$(MAKFILES)" ]; then \
	  echo "=== Formatting Makefiles ==="; \
	  cd $(ROOT) && $(MAKFORMAT) $(MAKFORMATARGS) $(MAKFILES); \
	fi

.PHONY: reformat
reformat:
	@if [ "$(BASE)" == "" ]; then \
	  echo "Please provide a base commit with \"make reformat BASE=<ref>\""; \
	else \
	  cd $(ROOT); git rebase --autostash --strategy-option=theirs --exec "make format BASE=HEAD~; git commit -a --amend --no-edit" $(BASE); \
	fi

$(call document_other_target,format,Format C++ and Python files. Apply on files specified explicitly (in CXXFILES and PYFILES variables) or on files modified since BASE (defaults to HEAD))
$(call document_other_target,reformat,Rebase and apply make format to all commits since BASE)
