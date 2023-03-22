.PHONY: poincare_log_visualization

# make EXECUTABLE=test poincare_log_visualization
# make SCENARIO=tests/calculation/calculation_addition.nws poincare_log_visualization

EXECUTABLE ?= epsilon
SCENARIO ?=

# TODO: remove TOOLCHAIN=host-clang to be able to build epsilon.app
MAKE_FLAGS := DEBUG=1 POINCARE_POOL_VISUALIZATION=1 PLATFORM=simulator ARCH=$(ARCH) TOOLCHAIN=host-clang EMBED_EXTRA_DATA=1
RUN_FLAGS ?=
EXECUTABLE_FILE = $(EXECUTABLE).app

ifneq ($(SCENARIO),)
EXECUTABLE_FILE = $(ARCH)/epsilon.bin
RUN_FLAGS = --load-state-file $(SCENARIO) --headless
endif

ifeq ($(EXECUTABLE),test)
EXECUTABLE_FILE = $(ARCH)/test.bin
RUN_FLAGS = --headless
endif


OUTPUT_LOGS := output/logs

.PHONY: $(OUTPUT_LOGS)/edition.xml
$(OUTPUT_LOGS)/cache.xml: $(OUTPUT_LOGS)/edition.xml # This is a workaround because grouped target (&:) is part of Make version > 4.2
$(OUTPUT_LOGS)/edition.xml:
	@echo "LOG     $@"
	@mkdir -p $(OUTPUT_LOGS)
	@$(MAKE) -j8 $(MAKE_FLAGS) $(notdir $(EXECUTABLE_FILE))
	@output/debug/simulator/macos/$(EXECUTABLE_FILE) $(RUN_FLAGS)

poincare_log_visualization: $(OUTPUT_LOGS)/cache.xml $(OUTPUT_LOGS)/edition.xml
	echo $(XML_FILES)
	@for arg in cache edition; do cp build/poincare_visualization/visualization.html $(OUTPUT_LOGS)/$${arg}.html; sed -i '' "s/XML_FILE/$${arg}.xml/g" $(OUTPUT_LOGS)/$${arg}.html; done
	@echo "In a browser, go to http://localhost:8000/*.html to visualize the cache logs"
	@cd $(OUTPUT_LOGS) && $(PYTHON) -m http.server 8000
