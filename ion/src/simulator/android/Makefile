ion_src += $(addprefix ion/src/simulator/android/src/cpp/, \
  haptics_enabled.cpp \
  platform_images.cpp \
  platform_language.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/journal.cpp \
  dummy/keyboard_callback.cpp \
  dummy/window_callback.cpp \
  circuit_breaker.cpp \
  clipboard_helper.cpp \
  haptics.cpp \
)

ion_src += ion/src/shared/collect_registers.cpp

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/android/src/cpp/telemetry.cpp
endif

$(call object_for,ion/src/simulator/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

LDFLAGS += -ljnigraphics -llog

# If ARCH is not defined, we will re-trigger a build for each avaialble ARCH.
# This is used to build APKs, which needs to embbed a binary for each ARCH.

ifndef ARCH

# Android resources
# Some android resources needs to be filtered through ImageMagick. Others are
# simply copied over.

$(BUILD_DIR)/app/res/mipmap/ic_launcher.png: ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background "#FFB734" $< $@

$(BUILD_DIR)/app/res/mipmap-v26/ic_launcher_foreground.png: ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background none $< -resize 512x512 -gravity center -background none -extent 1024x1024 $@

$(BUILD_DIR)/app/res/%.xml: ion/src/simulator/android/src/res/%.xml | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $< $@

# We need to put the .so files somewhere Gradle can pick them up.
# We decided to use the location "app/libs/$EXECUTABLE/$ARCH/libepsilon.so"
# This way it's easy to import the shared object from Java code (it's always
# named libepsilon.so), and it's easy to make Gradle use a given executable by
# simply using the jniLibs.src directive.
define path_for_arch_jni_lib
$$(BUILD_DIR)/app/libs/%/$(1)/libepsilon.so
endef

define rule_for_arch_jni_lib
$(call path_for_arch_jni_lib,$(1)): $$(BUILD_DIR)/$(1)/%.so | $$$$(@D)/.
	$(Q) cp $$< $$@
endef

$(foreach ARCH,$(ARCHS),$(eval $(call rule_for_arch_jni_lib,$(ARCH))))

apk_deps = $(foreach ARCH,$(ARCHS),$(call path_for_arch_jni_lib,$(ARCH)))
apk_deps += $(subst ion/src/simulator/android/src/res,$(BUILD_DIR)/app/res,$(wildcard ion/src/simulator/android/src/res/*/*))
apk_deps += $(addprefix $(BUILD_DIR)/app/res/,mipmap/ic_launcher.png mipmap-v26/ic_launcher_foreground.png)

.PRECIOUS: $(apk_deps) $(simulator_app_deps)

$(BUILD_DIR)/%.apk: $(apk_deps) $(simulator_app_deps)
	$(call rule_label,GRADLE)
	$(Q) ANDROID_HOME=$(ANDROID_HOME) EPSILON_VERSION=$(EPSILON_VERSION) BUILD_DIR=$(BUILD_DIR) NDK_BUNDLE_VERSION=$(NDK_BUNDLE_VERSION) EPSILON_VARIANT=$* ion/src/simulator/android/gradlew -b ion/src/simulator/android/build.gradle assembleRelease
	$(Q) cp $(BUILD_DIR)/app/outputs/apk/release/android-release*.apk $@

endif
