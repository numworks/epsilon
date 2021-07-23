# Coverage

coverage_src := $(filter-out apps/main.cpp ion/src/simulator/external/%,$(sort $(apps_src) $(base_src)))
$(call flavored_object_for,$(coverage_src),consoledisplay): SFLAGS += --coverage
coverage_src += $(test_runner_src) $(runner_src) $(tests_src)
to_remove = quiz/src/i18n.cpp \
 apps/exam_mode_configuration_non_official.cpp \
 apps/exam_mode_configuration_non_official.cpp:-official
coverage_src := $(filter-out $(to_remove),$(sort $(coverage_src)))
$(BUILD_DIR)/test_coverage.$(EXE): LDFLAGS += --coverage
$(BUILD_DIR)/test_coverage.$(EXE): $(call flavored_object_for,$(coverage_src),consoledisplay)

coverage.info: $(BUILD_DIR)/test_coverage.$(EXE)
	$(Q) ./$< --headless --limit-stack-usage
	$(Q) lcov --capture --no-external --directory . --output-file $@

PRECIOUS: coverage
coverage: coverage.info
	$(Q) genhtml $< -s --legend --output-directory output/$@
	$(Q) open output/$@/index.html
