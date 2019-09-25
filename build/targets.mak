# Define standard Epsilon targets
all_epsilon_common_src = $(ion_src) $(liba_src) $(kandinsky_src) $(epsilon_src) $(app_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(ion_device_dfu_relocated_src)
all_epsilon_default_src = $(all_epsilon_common_src) $(apps_launch_default_src) $(apps_prompt_none_src)

$(BUILD_DIR)/epsilon.$(EXE): $(call object_for,$(all_epsilon_default_src))
$(BUILD_DIR)/epsilon.on-boarding.$(EXE): $(call object_for,$(all_epsilon_common_src) $(apps_launch_on_boarding_src) $(apps_prompt_none_src))
$(BUILD_DIR)/epsilon.on-boarding.update.$(EXE): $(call object_for,$(all_epsilon_common_src) $(apps_launch_on_boarding_src) $(apps_prompt_update_src))
$(BUILD_DIR)/epsilon.on-boarding.beta.$(EXE): $(call object_for,$(all_epsilon_common_src) $(apps_launch_on_boarding_src) $(apps_prompt_beta_src))

$(BUILD_DIR)/test.$(EXE): $(BUILD_DIR)/quiz/src/tests_symbols.o $(call object_for,$(ion_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(ion_device_dfu_relocated_src) $(tests_src) $(runner_src) $(app_calculation_test_src) $(app_probability_test_src) $(app_regression_test_src) $(app_sequence_test_src) $(app_shared_test_src) $(app_statistics_test_src) $(app_solver_test_src))

# Define handy targets
# Those can be built easily by simply invoking "make target.ext". The named file
# will be built in $(BUILD_DIR).

HANDY_TARGETS = epsilon epsilon.on-boarding epsilon.on-boarding.update epsilon.on-boarding.beta test
HANDY_TARGETS_EXTENSIONS = $(EXE)

define handy_target_rule
.PHONY: $(1).$(2)
$(1).$(2): $$(BUILD_DIR)/$(1).$(2)
endef

# Load platform-specific targets
# We include them before the standard ones to give them precedence.
-include build/targets.$(PLATFORM).mak

$(foreach extension,$(HANDY_TARGETS_EXTENSIONS),$(foreach executable,$(HANDY_TARGETS),$(eval $(call handy_target_rule,$(executable),$(extension)))))
