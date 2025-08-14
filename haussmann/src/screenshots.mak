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
