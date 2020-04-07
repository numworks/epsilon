# Define handy targets
# Those can be built easily by simply invoking "make target.ext". The named file
# will be built in $(BUILD_DIR).

HANDY_TARGETS ?=
HANDY_TARGETS_EXTENSIONS ?=

# Epsilon base target

base_src = $(ion_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src)

epsilon_src = $(base_src) $(apps_src)

$(BUILD_DIR)/epsilon.$(EXE): $(call flavored_object_for,$(epsilon_src))

HANDY_TARGETS += epsilon

# Epsilon flavored targets

epsilon_flavors = \
  onboarding \
  onboarding.update \
  onboarding.beta

define rule_for_epsilon_flavor
$$(BUILD_DIR)/epsilon.$(1).$$(EXE): $$(call flavored_object_for,$$(epsilon_src),$(1))
endef

$(foreach flavor,$(epsilon_flavors),$(eval $(call rule_for_epsilon_flavor,$(flavor))))

HANDY_TARGETS += $(foreach flavor,$(epsilon_flavors),epsilon.$(flavor))

# Epsilon official targets

epsilon_official_flavors = \
  official \
  official.onboarding \
  official.onboarding.update \
  official.onboarding.beta

define rule_for_unconfirmed_official_flavor
$$(BUILD_DIR)/epsilon.$(1).$$(EXE):
	@echo "CAUTION: You are trying to build an official NumWorks firmware."
	@echo "Distribution of such firmware by a third party is prohibited."
	@echo "Please set the ACCEPT_OFFICIAL_TOS environment variable to proceed."
	@exit -1
endef

ifeq ($(ACCEPT_OFFICIAL_TOS),1)
rule_for_official_epsilon_flavor = rule_for_epsilon_flavor
else
rule_for_official_epsilon_flavor = rule_for_unconfirmed_official_flavor
endif

$(foreach flavor,$(epsilon_official_flavors),$(eval $(call $(rule_for_official_epsilon_flavor),$(flavor))))

HANDY_TARGETS += $(foreach flavor,$(epsilon_official_flavors),epsilon.$(flavor))

# Test

test_runner_src = $(base_src) $(apps_tests_src) $(runner_src) $(tests_src)

$(BUILD_DIR)/test.$(EXE): $(call flavored_object_for,$(test_runner_src),consoledisplay)

HANDY_TARGETS += test

# Load platform-specific targets
# We include them before the standard ones to give them precedence.
-include build/targets.$(PLATFORM).mak

# Generate handy targets rules
# Define handy targets
# Those can be built easily by simply invoking "make target.ext". The named file
# will be built in $(BUILD_DIR).

HANDY_TARGETS_EXTENSIONS += $(EXE)

define handy_target_rule
.PHONY: $(1).$(2)
$(1).$(2): $$(BUILD_DIR)/$(1).$(2)
endef

$(foreach extension,$(HANDY_TARGETS_EXTENSIONS),$(foreach executable,$(HANDY_TARGETS),$(eval $(call handy_target_rule,$(executable),$(extension)))))
