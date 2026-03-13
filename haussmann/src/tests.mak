_compare_script := tools/screenshots/compare_crc.py

_compare_args := $(ARGS)
_compare_args ?= --dataset tests/screenshots_dataset

ifneq ($(findstring $( ),$(ARCHS)),)
%.compare:
	$(error Several archs exist for platform, select one by overriding the ARCHS variable)
else
%.compare: $(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)%.$(EXECUTABLE_EXTENSION)
	$(PYTHON) $(_compare_script) $(_compare_args) $<
endif

$(call document_extension,compare,Run the screenshot tests. Edit ARGS to pass arguments (e.g. ARGS="--help"))


%_test.$(EXECUTABLE_EXTENSION).run_parallel: $(OUTPUT_DIRECTORY)/$(if $(ARCHS),$(ARCHS)/,)%_test.$(EXECUTABLE_EXTENSION)
	set -e -o pipefail; N=$$((parallel --will-cite --halt now,fail=1 --keep-order $^ --headless --limit-stack-usage --number-of-chunks 32 --chunk-id {} ::: $$(seq 0 31) || exit 1) | tee /dev/tty | wc -l); echo "ALL $$N TESTS FINISHED"

$(call document_extension,run_parallel,Run the unit tests concurrently using `parallel`.)
