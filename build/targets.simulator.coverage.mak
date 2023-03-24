# Coverage

coverage_src := $(sort $(apps_src) $(base_src)))
to_remove = \
 apps/exam_mode_configuration_non_official.cpp \
 apps/exam_mode_configuration_non_official.cpp:-official \
 apps/apps_container_helper.cpp \
 apps/init.cpp
coverage_src := $(filter-out $(to_remove),$(sort $(coverage_src)))
coverage_unary_src := $(filter-out apps/main.cpp ion/src/simulator/external/%,$(coverage_src)))
coverage_unary_src += $(filter-out quiz/src/i18n.cpp,$(test_runner_src))
coverage_integration_src := $(coverage_src)

$(call flavored_object_for,$(coverage_unary_src) $(coverage_integration_src),consoledisplay): SFLAGS += --coverage

$(BUILD_DIR)/test_%_coverage.$(EXE): LDFLAGS += --coverage
$(BUILD_DIR)/test_unary_coverage.$(EXE): $(call flavored_object_for,$(coverage_unary_src),consoledisplay)
$(BUILD_DIR)/test_integration_coverage.$(EXE): $(call flavored_object_for,$(coverage_integration_src),consoledisplay)

coverage.unary.info: $(BUILD_DIR)/test_unary_coverage.$(EXE)
	$(Q) ./$< --headless --limit-stack-usage
	$(Q) lcov --capture --directory $(BUILD_DIR) --output-file $@

coverage.integration.info: $(BUILD_DIR)/test_integration_coverage.$(EXE)
	for state_file in tests/screenshots_dataset/scenari/*.nws; do ./$< --headless --limit-stack-usage --load-state-file $$state_file; done
	lcov --capture --directory $(BUILD_DIR) --output-file $@

PRECIOUS: coverage_unary coverage_integration
coverage_%: coverage.%.info
	$(Q) genhtml $< -s --legend --output-directory output/$@
	$(Q) open output/$@/index.html
