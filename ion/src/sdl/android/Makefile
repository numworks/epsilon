ion_src += $(addprefix ion/src/sdl/android/src/cpp/, \
  images.cpp \
  language.cpp \
  telemetry.cpp \
)

$(call object_for,ion/src/sdl/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

LDFLAGS += -ljnigraphics -llog

# If NDK_ABI is not defined, we will re-trigger a build for each avaialble ABI.
# This is used to build APKs, which needs to embbed a binary for each ABI.

ifdef NDK_ABI

.PHONY: epsilon.apk
epsilon.apk:
	echo "To make epsilon.apk, to not define NDK_ABI"

else

# Cross-ABI libepsilon.so
# This file is loaded is loaded only once, which prevents us from tracking
# dependencies across ABIs. As a shortcut, we simply force a re-make of
# libepsilon.so for each ABI.

.PHONY: force_remake

$(BUILD_DIR)/app/libs/%/libepsilon.so: force_remake $$(@D)/.
	$(Q) echo "MAKE    NDK_ABI=$*"
	$(Q) $(MAKE) NDK_ABI=$* epsilon.so
	$(Q) cp $(BUILD_DIR)/$*/epsilon.so $@

# Android resources
# Some android resources needs to be filtered through ImageMagick. Others are
# simply copied over.

$(BUILD_DIR)/app/res/mipmap/ic_launcher.png: ion/src/sdl/assets/logo.svg | $$(@D)/.
	@echo "CONVERT $@"
	$(Q) convert -background "#FFB734" $< $@

$(BUILD_DIR)/app/res/mipmap-v26/ic_launcher_foreground.png: ion/src/sdl/assets/logo.svg | $$(@D)/.
	@echo "CONVERT $@"
	$(Q) convert -background none $< -resize 512x512 -gravity center -background none -extent 1024x1024 $@

$(BUILD_DIR)/app/res/%: ion/src/sdl/android/src/res/% $$(@D)/.
	@echo "COPY    $@"
	$(Q) cp $< $@

NDK_ABIS = armeabi-v7a arm64-v8a x86 x86_64

epsilon_apk_deps = $(patsubst %,$(BUILD_DIR)/app/libs/%/libepsilon.so,$(NDK_ABIS))
epsilon_apk_deps += $(subst ion/src/sdl/android/src/res,$(BUILD_DIR)/app/res,$(wildcard ion/src/sdl/android/src/res/*/*))
epsilon_apk_deps += $(addprefix $(BUILD_DIR)/app/res/,mipmap/ic_launcher.png mipmap-v26/ic_launcher_foreground.png)

.PHONY: gradle_%
gradle_%: $(epsilon_apk_deps)
	@echo "GRADLE ion/src/sdl/android/build.gradle"
	$(Q) ANDROID_HOME=/usr/local/android EPSILON_VERSION=$(EPSILON_VERSION) gradle -b ion/src/sdl/android/build.gradle $*

.PHONY: epsilon.apk
epsilon.apk: gradle_assembleRelease

.PHONY: epsilon_run
epsilon_run: gradle_installDebug

endif
