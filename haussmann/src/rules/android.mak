%.apk: $(OUTPUT_DIRECTORY)/%.apk
	@ :

# We need to put the .so files somewhere Gradle can pick them up.
# We decided to use the location "app/libs/$EXECUTABLE/$ARCH/libepsilon.so"
# This way it's easy to import the shared object from Java code (it's always
# named libepsilon.so), and it's easy to make Gradle use a given executable by
# simply using the jniLibs.src directive.
define path_for_arch_jni_lib
$$(OUTPUT_DIRECTORY)/app/libs/%/$(1)/libepsilon.so
endef

define rule_for_arch_jni_lib
$(call path_for_arch_jni_lib,$(1)): $$(OUTPUT_DIRECTORY)/$(1)/%.so | $$$$(@D)/.
	$(Q) cp $$< $$@
endef

$(foreach ARCH,$(ARCHS),$(eval $(call rule_for_arch_jni_lib,$(ARCH))))

$(OUTPUT_DIRECTORY)/%.apk: $$(_ion_simulator_assets) $(foreach ARCH,$(ARCHS),$(call path_for_arch_jni_lib,$(ARCH))) $$(_ion_apk_deps)
	$(call rule_label,GRADLE)
	../shared/ion/src/simulator/android/gradlew -p ../shared/ion/src/simulator/android -PappVersion=$(APP_VERSION) -PoutputDirectory=$$(realpath $(OUTPUT_DIRECTORY)) -PgoalName=$(basename $(@F)) -PndkBundleVersion=$(NDK_BUNDLE_VERSION) assembleRelease
	cp $(OUTPUT_DIRECTORY)/app/outputs/apk/release/android-release*.apk $@

$(call document_extension,apk)

%_aligned.apk: $(OUTPUT_DIRECTORY)/%_aligned.apk
	:

$(OUTPUT_DIRECTORY)/%_aligned.apk: $(OUTPUT_DIRECTORY)/%.apk
	$(call rule_label,ZIPALGN)
	$(BUILD_TOOLS_PATH)/zipalign -f 4 $< $@.temp; $(BUILD_TOOLS_PATH)/zipalign -f 4 $@.temp $@


$(call document_other_target,<project>_aligned.apk,Zipaligned APK)

%_signed.apk: $(OUTPUT_DIRECTORY)/%_signed.apk
	:

$(OUTPUT_DIRECTORY)/%_signed.apk: $(OUTPUT_DIRECTORY)/%_aligned.apk
	$(call rule_label,APKSIGN)
	$(BUILD_TOOLS_PATH)/apksigner sign --ks $(KEYSTORE) --out $@ $<

$(call document_other_target,<project>_signed.apk,Signed APK ready to be installed)

$(OUTPUT_DIRECTORY)/%_signed.apk_flash: $(OUTPUT_DIRECTORY)/%_signed.apk
	$(call rule_label,ADBFLASH)
	adb install $<

%_signed.apk_flash: $(OUTPUT_DIRECTORY)/%_signed.apk_flash
	:

$(call document_other_target,<project>_signed.apk_flash,Flash a signed APK with ADB)

%.aab: $(OUTPUT_DIRECTORY)/%.aab
	@ :

$(OUTPUT_DIRECTORY)/%.aab: $$(_ion_simulator_assets) $(foreach ARCH,$(ARCHS),$(call path_for_arch_jni_lib,$(ARCH))) $$(_ion_apk_deps)
	$(if $(KEYSTORE),,$(error KEYSTORE must be set to build a signed AAB))
	$(call rule_label,GRADLE)
	@printf 'Keystore password: '; read -rs ks_password; echo; \
	ANDROID_SIGNING_STORE_FILE=$$(realpath $(KEYSTORE)) ANDROID_SIGNING_STORE_PASSWORD=$$ks_password ANDROID_SIGNING_KEY_ALIAS=numworks ANDROID_SIGNING_KEY_PASSWORD=$$ks_password $(PATH_ion)/src/simulator/android/gradlew -p $(PATH_ion)/src/simulator/android -PappVersion=$(APP_VERSION) -PoutputDirectory=$$(realpath $(OUTPUT_DIRECTORY)) -PgoalName=$(basename $(@F)) -PndkBundleVersion=$(NDK_BUNDLE_VERSION) bundleRelease
	cp $(OUTPUT_DIRECTORY)/app/outputs/bundle/release/android-release.aab $@

$(call document_extension,aab)

$(OUTPUT_DIRECTORY)/%.apks: $(OUTPUT_DIRECTORY)/%.aab
	$(if $(KEYSTORE),,$(error KEYSTORE must be set to build an APK set))
	$(call rule_label,BUNDLETOOL)
	bundletool build-apks --bundle=$< --output=$@ --ks=$(KEYSTORE) --ks-key-alias=numworks --overwrite

$(OUTPUT_DIRECTORY)/%.aab_flash: $(OUTPUT_DIRECTORY)/%.apks
	$(call rule_label,AABFLASH)
	bundletool install-apks --apks=$<

%.aab_flash: $(OUTPUT_DIRECTORY)/%.aab_flash
	:

$(call document_other_target,<project>.aab_flash,Install an AAB on a connected device via bundletool)

