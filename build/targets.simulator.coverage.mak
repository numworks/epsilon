# Coverage

to_remove = \
 apps/apps_container_helper.cpp \
 apps/init.cpp \
 apps/main.cpp \
 quiz/src/i18n.cpp
coverage_unit_src := $(filter-out $(to_remove),$(epsilon_src) $(test_runner_src))
coverage_integration_src := $(epsilon_src)

$(call flavored_object_for,$(coverage_unit_src) $(coverage_integration_src),consoledisplay): SFLAGS += --coverage

$(BUILD_DIR)/%/test_coverage.$(EXE): LDFLAGS += --coverage
$(BUILD_DIR)/unit/test_coverage.$(EXE): $(call flavored_object_for,$(coverage_unit_src),consoledisplay)
$(BUILD_DIR)/integration/test_coverage.$(EXE): $(call flavored_object_for,$(coverage_integration_src),consoledisplay)

to_exclude = \
  '**/eadk/**' \
  '**/ion/src/external/**' \
  '**/ion/src/simulator/external/**' \
  '*/output/**' \
  '**/python/src/**' \
  '/Applications/**' \
  '/Library/**'

$(BUILD_DIR)/coverage.info: $(BUILD_DIR)/unit/test_coverage.$(EXE) $(BUILD_DIR)/integration/test_coverage.$(EXE)
	$(Q) ./$< --headless --limit-stack-usage
	$(Q) for state_file in tests/screenshots_dataset/scenari/*.nws; do ./$(word 2,$^) --headless --limit-stack-usage --load-state-file $$state_file; done
	$(Q) lcov --capture --directory $(BUILD_DIR) --output-file $@
	$(Q) lcov --remove $@ $(to_exclude) -o $@

.PRECIOUS: coverage
coverage: $(BUILD_DIR)/coverage.info
	$(Q) genhtml $< -s --legend --output-directory $(BUILD_DIR)/$@
	$(Q) open  $(BUILD_DIR)/$@/index.html
