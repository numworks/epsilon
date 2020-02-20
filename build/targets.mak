# Define standard Epsilon targets
base_src = $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src)

epsilon_src = $(base_src) $(ion_default_src) $(apps_default_src)
epsilon_official_src = $(base_src) $(ion_default_src) $(apps_official_default_src)

$(BUILD_DIR)/epsilon.$(EXE): $(call object_for,$(epsilon_src))
$(BUILD_DIR)/epsilon.official.$(EXE): $(call object_for,$(epsilon_official_src))
$(BUILD_DIR)/epsilon.onboarding.$(EXE): $(call object_for, $(base_src) $(ion_default_src) $(apps_onboarding_src))
$(BUILD_DIR)/epsilon.official.onboarding.$(EXE): $(call object_for,$(base_src) $(ion_default_src) $(apps_official_onboarding_src))
$(BUILD_DIR)/epsilon.onboarding.update.$(EXE): $(call object_for, $(base_src) $(ion_default_src) $(apps_onboarding_update_src))
$(BUILD_DIR)/epsilon.official.onboarding.update.$(EXE): $(call object_for,$(base_src) $(ion_default_src) $(apps_official_onboarding_update_src))
$(BUILD_DIR)/epsilon.onboarding.beta.$(EXE): $(call object_for, $(base_src) $(ion_default_src) $(apps_onboarding_beta_src))
$(BUILD_DIR)/epsilon.official.onboarding.beta.$(EXE): $(call object_for,$(base_src) $(ion_default_src) $(apps_official_onboarding_beta_src))

test_base_src = $(base_src) $(apps_tests_src) $(runner_src) $(tests_src)

test_runner_src = $(test_base_src) $(ion_console_on_screen_src)
$(BUILD_DIR)/test.$(EXE): $(call object_for,$(test_runner_src))

# Define handy targets
# Those can be built easily by simply invoking "make target.ext". The named file
# will be built in $(BUILD_DIR).

HANDY_TARGETS += epsilon epsilon.official epsilon.onboarding epsilon.official.onboarding epsilon.onboarding.update epsilon.official.onboarding.update epsilon.onboarding.beta epsilon.official.onboarding.beta test
HANDY_TARGETS_EXTENSIONS += $(EXE)

define handy_target_rule
.PHONY: $(1).$(2)
$(1).$(2): $$(BUILD_DIR)/$(1).$(2)
endef

# Load platform-specific targets
# We include them before the standard ones to give them precedence.
-include build/targets.$(PLATFORM).mak

$(foreach extension,$(HANDY_TARGETS_EXTENSIONS),$(foreach executable,$(HANDY_TARGETS),$(eval $(call handy_target_rule,$(executable),$(extension)))))

include build/targets.all.mak
