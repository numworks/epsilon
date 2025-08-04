%.apk: $(OUTPUT_DIRECTORY)/%.apk
	@ :

# We need to put the .so files somewhere Gradle can pick them up.
# We decided to use the location "app/libs/$EXECUTABLE/$ARCH/libepsilon.so"
# This way it's easy to import the shared object from Java code (it's always
# named libepsilon.so), and it's easy to make Gradle use a given executable by
# simply using the jniLibs.src directive.
define path_for_arch_jni_lib
$$(OUTPUT_DIRECTORY)/app/libs/epsilon/$(1)/libepsilon.so
endef

define rule_for_arch_jni_lib
$(call path_for_arch_jni_lib,$(1)): $$(OUTPUT_DIRECTORY)/$(1)/epsilon.so | $$$$(@D)/.
	$(Q) cp $$< $$@
endef

$(foreach ARCH,$(ARCHS),$(eval $(call rule_for_arch_jni_lib,$(ARCH))))

apk_deps = $(foreach ARCH,$(ARCHS),$(call path_for_arch_jni_lib,$(ARCH)))
apk_deps += $(subst ion/src/simulator/android/src/res,$(OUTPUT_DIRECTORY)/app/res,$(wildcard ion/src/simulator/android/src/res/*/*))
apk_deps += $(addprefix $(OUTPUT_DIRECTORY)/app/res/,mipmap/ic_launcher.png mipmap-v26/ic_launcher_foreground.png)

$(OUTPUT_DIRECTORY)/%.apk: $$(simulator_app_deps) $(apk_deps)
	$(call rule_label,GRADLE)
	$(Q) ion/src/simulator/android/gradlew -b ion/src/simulator/android/build.gradle -PappVersion=$(APP_VERSION) -PoutputDirectory=$$(realpath $(OUTPUT_DIRECTORY)) -PndkBundleVersion=$(NDK_BUNDLE_VERSION) assembleRelease
	$(Q) cp $(OUTPUT_DIRECTORY)/app/outputs/apk/release/android-release*.apk $@

$(call document_extension,apk)
