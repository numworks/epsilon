ion_src += $(addprefix ion/src/simulator/android/src/cpp/, \
  images.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/callback.cpp \
  dummy/language.cpp \
)

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/android/src/cpp/telemetry.cpp
endif

$(call object_for,ion/src/simulator/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

LDFLAGS += -ljnigraphics -llog

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

# Cross-ABI libepsilon.so
# This file is loaded is loaded only once, which prevents us from tracking
# dependencies across ABIs. As a shortcut, we simply force a re-make of
# libepsilon.so for each ABI.

.PHONY: force_remake

define rule_for_libepsilon
$$(BUILD_DIR)/app/libs/%/lib$(1): force_remake $$$$(@D)/.
	$(Q) echo "MAKE    NDK_ABI=$$*"
	$(Q) $$(MAKE) NDK_ABI=$$* $(1)
	$(Q) cp $$(BUILD_DIR)/$$*/$(1) $$@
endef

$(eval $(call rule_for_libepsilon,epsilon.so))
$(eval $(call rule_for_libepsilon,epsilon.official.so))

# If NDK_ABI is not defined, we will re-trigger a build for each avaialble ABI.
# This is used to build APKs, which needs to embbed a binary for each ABI.

ifndef NDK_ABI

NDK_ABIS = armeabi-v7a arm64-v8a x86 x86_64

epsilon_apk_deps = $(subst ion/src/simulator/android/src/res,$(BUILD_DIR)/app/res,$(wildcard ion/src/simulator/android/src/res/*/*))
epsilon_apk_deps += $(addprefix $(BUILD_DIR)/app/res/,mipmap/ic_launcher.png mipmap-v26/ic_launcher_foreground.png)

define rule_for_gradle
.PHONY: gradle_$1_$2
gradle_$1_$2: $$(epsilon_apk_deps) $$(patsubst %,$$(BUILD_DIR)/app/libs/%/libepsilon$2so,$(NDK_ABIS))
	@echo "GRADLE ion/src/simulator/android/build.gradle"
	$(Q) ANDROID_HOME=$(ANDROID_HOME) EPSILON_VERSION=$(EPSILON_VERSION) BUILD_DIR=$(BUILD_DIR) ion/src/simulator/android/gradlew -b ion/src/simulator/android/build.gradle $1
endef

$(eval $(call rule_for_gradle,assembleCodesigned,.))
$(eval $(call rule_for_gradle,assembleRelease,.))
$(eval $(call rule_for_gradle,assembleCodesigned,.official.))
$(eval $(call rule_for_gradle,assembleRelease,.official.))

DEFAULT = epsilon.apk

.PHONY: epsilon%signed.apk
epsilon%signed.apk: gradle_assembleCodesigned_%
	$(warning This is a signed build.)

.PHONY: epsilon%apk
epsilon%apk: gradle_assembleRelease_%
	$(warning Building without code signing. Build epsilon$*signed.apk to generate a signed version.)

.PHONY: epsilon_run
epsilon_run: gradle_installDebug

endif
