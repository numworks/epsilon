# Define standard Epsilon targets
base_src = $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src)

epsilon_src = $(base_src) $(ion_default_src) $(apps_default_src)
epsilon_official_src = $(base_src) $(ion_default_src) $(apps_official_default_src)
epsilon_onboarding_src = $(base_src) $(ion_default_src) $(apps_onboarding_src)
epsilon_official_onboarding_src = $(base_src) $(ion_default_src) $(apps_official_onboarding_src)
epsilon_onboarding_update_src = $(base_src) $(ion_default_src) $(apps_onboarding_update_src)
epsilon_official_onboarding_update_src = $(base_src) $(ion_default_src) $(apps_official_onboarding_update_src)
epsilon_onboarding_beta_src = $(base_src) $(ion_default_src) $(apps_onboarding_beta_src)
epsilon_official_onboarding_beta_src = $(base_src) $(ion_default_src) $(apps_official_onboarding_beta_src)

$(BUILD_DIR)/epsilon.$(EXE): $(call object_for,$(epsilon_src))
$(BUILD_DIR)/epsilon.official.$(EXE): $(call object_for,$(epsilon_official_src))
$(BUILD_DIR)/epsilon.onboarding.$(EXE): $(call object_for,$(epsilon_onboarding_src))
$(BUILD_DIR)/epsilon.official.onboarding.$(EXE): $(call object_for,$(epsilon_official_onboarding_src))
$(BUILD_DIR)/epsilon.onboarding.update.$(EXE): $(call object_for,$(epsilon_onboarding_update_src))
$(BUILD_DIR)/epsilon.official.onboarding.update.$(EXE): $(call object_for,$(epsilon_official_onboarding_update_src))
$(BUILD_DIR)/epsilon.onboarding.beta.$(EXE): $(call object_for,$(epsilon_onboarding_beta_src))
$(BUILD_DIR)/epsilon.official.onboarding.beta.$(EXE): $(call object_for,$(epsilon_official_onboarding_beta_src))

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

define source_emsdk
source ~/emsdk/emsdk_env.sh > /dev/null
endef

ANDROID_GRADLE_KEYSTORE:= $(wildcard ~/.gradle/google-play-upload.keystore)
ANDROID_GRADLE_PROPERTIES:= $(wildcard ~/.gradle/gradle.properties)
IOS_MOBILE_PROVISION:= $(wildcard ~/Downloads/NumWorks_Graphing_Calculator_Distribution.mobileprovision)

MISSING_FILES=1

ifneq "$(and $(ANDROID_GRADLE_KEYSTORE),$(ANDROID_GRADLE_PROPERTIES))" ""
ifneq "$(IOS_MOBILE_PROVISION)" ""
  MISSING_FILES=0
endif
endif

.PHONY: stable_release
ifeq ($(MISSING_FILES),1)
stable_release:
	@echo "Some required files for app signing are missing among: ~/.gradle/google-play-upload.keystore, ~/.gradle/gradle.properties or ~/Downloads/NumWorks_Graphing_Calculator_Distribution.mobileprovision."
else
stable_release:
	$(Q) rm -rf output/stable_release
	$(Q) mkdir -p output/stable_release
	$(Q) echo "BUILD_FIRMWARE    DEVICE N0110"
	$(Q) make clean
	$(Q) make -j8 epsilon.official.onboarding.dfu
	$(Q) cp output/release/device/n0110/epsilon.official.onboarding.dfu output/stable_release/epsilon.device.n0110.dfu
	$(Q) echo "BUILD_FIRMWARE    DEVICE N0100"
	$(Q) make MODEL=n0100 clean
	$(Q) make -j8 MODEL=n0100 epsilon.official.onboarding.dfu
	$(Q) cp output/release/device/n0100/epsilon.official.onboarding.dfu output/stable_release/epsilon.device.n0100.dfu
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR WEB ZIP"
	$(Q) make -j8 DEBUG=0 PLATFORM=simulator TARGET=web clean
	$(Q) $(call source_emsdk); make -j8 DEBUG=0 PLATFORM=simulator TARGET=web output/release/simulator/web/simulator.official.zip
	$(Q) cp output/release/simulator/web/simulator.official.zip output/stable_release/simulator.web.zip
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR WEB JS"
	$(Q) $(call source_emsdk); make -j8 DEBUG=0 PLATFORM=simulator TARGET=web epsilon.official.js
	$(Q) cp output/release/simulator/web/epsilon.official.js output/stable_release/epsilon.js
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR WEB PYTHON JS"
	$(Q) make -j8 DEBUG=0 PLATFORM=simulator TARGET=web clean
	$(Q) $(call source_emsdk); make -j8 DEBUG=0 PLATFORM=simulator TARGET=web EPSILON_GETOPT=1 EPSILON_APPS=code epsilon.official.js
	$(Q) cp output/release/simulator/web/epsilon.official.js output/stable_release/epsilon.python.js
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR ANDROID"
	$(Q) make -j8 PLATFORM=simulator TARGET=android clean
	$(Q) make -j8 PLATFORM=simulator TARGET=android epsilon.official.apk
#TODO: what are the files made by gradle?
#	$(Q) cp output/release/simulator/android/epsilon.official.apk output/stable_release/epsilon.apk
	$(Q) echo "BUILD_FIRMWARE    SIMULATOR IOS"
	$(Q) make -j8 PLATFORM=simulator TARGET=ios clean
	$(Q) make -j8 PLATFORM=simulator TARGET=ios IOS_PROVISIONNING_PROFILE="~/Downloads/NumWorks_Graphing_Calculator_Distribution.mobileprovision" output/release/simulator/ios/app/epsilon.official.ipa
#TODO: which files are generated by codesign?
#	$(Q) cp output/release/simulator/ios/app/epsilon.official.ipa output/stable_release/epsilon.ipa
endif
