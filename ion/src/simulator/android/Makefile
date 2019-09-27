ion_src += $(addprefix ion/src/simulator/android/src/cpp/, \
  images.cpp \
  telemetry.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/callback.cpp \
  dummy/language.cpp \
)

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

$(BUILD_DIR)/app/libs/%/libepsilon.so: force_remake $$(@D)/.
	$(Q) echo "MAKE    NDK_ABI=$*"
	$(Q) $(MAKE) NDK_ABI=$* epsilon.so
	$(Q) cp $(BUILD_DIR)/$*/epsilon.so $@

# If NDK_ABI is not defined, we will re-trigger a build for each avaialble ABI.
# This is used to build APKs, which needs to embbed a binary for each ABI.

ifndef NDK_ABI

NDK_ABIS = armeabi-v7a arm64-v8a x86 x86_64

epsilon_apk_deps = $(patsubst %,$(BUILD_DIR)/app/libs/%/libepsilon.so,$(NDK_ABIS))
epsilon_apk_deps += $(subst ion/src/simulator/android/src/res,$(BUILD_DIR)/app/res,$(wildcard ion/src/simulator/android/src/res/*/*))
epsilon_apk_deps += $(addprefix $(BUILD_DIR)/app/res/,mipmap/ic_launcher.png mipmap-v26/ic_launcher_foreground.png)

.PHONY: gradle_%
gradle_%: $(epsilon_apk_deps)
	@echo "GRADLE ion/src/simulator/android/build.gradle"
	$(Q) ANDROID_HOME=$(ANDROID_HOME) EPSILON_VERSION=$(EPSILON_VERSION) BUILD_DIR=$(BUILD_DIR) ion/src/simulator/android/gradlew -b ion/src/simulator/android/build.gradle $*

DEFAULT = epsilon.apk

.PHONY: epsilon.apk
ifdef ANDROID_SIGNING_STORE_FILE
epsilon.apk: gradle_assembleCodesigned
else
epsilon.apk: gradle_assembleRelease
	$(warning Building without code signing. Define ANDROID_SIGNING_STORE_FILE, ANDROID_SIGNING_STORE_PASSWORD, ANDROID_SIGNING_KEY_ALIAS and  ANDROID_SIGNING_KEY_PASSWORD for a signed build.)
endif


.PHONY: epsilon_run
epsilon_run: gradle_installDebug

endif
