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
$(OUTPUT_LOGS)/edition.xml:
	@echo "LOG     $@"
	@mkdir -p $(OUTPUT_LOGS)
	@$(MAKE) -j8 $(MAKE_FLAGS) $(notdir $(EXECUTABLE_FILE))
	@output/debug/simulator/macos/$(EXECUTABLE_FILE) $(RUN_FLAGS)

poincare_log_visualization: $(OUTPUT_LOGS)/edition.xml
	@cp build/poincare_visualization/visualization.html $(OUTPUT_LOGS)/edition.html; sed -i '' "s/XML_FILE/edition.xml/g" $(OUTPUT_LOGS)/edition.html;
	@echo "In a browser, go to http://localhost:8000/edition.html to visualize the logs"
	$(PYTHON) -m http.server 8000 --directory $(OUTPUT_LOGS)
