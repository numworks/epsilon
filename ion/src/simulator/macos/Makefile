ION_SIMULATOR_FILES = 1

ion_src += $(addprefix ion/src/simulator/macos/, \
  platform_files.mm \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  apple/platform_images.mm \
  apple/platform_language.mm \
  dummy/haptics_enabled.cpp \
  dummy/keyboard_callback.cpp \
  dummy/window_callback.cpp \
  unix/platform_files.cpp \
  circuit_breaker.cpp \
  clipboard_helper.cpp \
  haptics.cpp \
  journal.cpp \
)

ifeq ($(ARCH),x86_64)
ion_src += ion/src/simulator/shared/collect_registers.cpp
ion_src += ion/src/simulator/shared/collect_registers_x86_64.s
else
ion_src += ion/src/shared/collect_registers.cpp
endif

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/shared/dummy/telemetry_init.cpp
ion_src += ion/src/shared/telemetry_console.cpp
endif

ifndef ARCH

# App resources

SIMULATOR_ICON_SIZES = 16x16 32x32 64x64 128x128 256x256 512x512 1024x1024
SIMULATOR_ICON_USE_MASK = 1
SIMULATOR_APP_BINARY_PATH = Contents/MacOS/
SIMULATOR_APP_RESOURCE_PATH = Contents/Resources/
SIMULATOR_APP_PLIST_PATH = Contents/
SIMULATOR_ICONSET = $(BUILD_DIR)/app/assets/app.iconset

include ion/src/simulator/shared/apple/helpers.mak

$(call simulator_app_plist,Info.plist): ion/src/simulator/macos/Info.plist | $$(@D)/.
	$(call rule_label,PLUTIL)
	$(Q) cp $< $@
	$(Q) plutil -insert "LSMinimumSystemVersion" -string "$(MACOS_MIN_VERSION)" $@
	$(Q) plutil -insert "CFBundleVersion" -string "$(EPSILON_VERSION)" $@
	$(Q) plutil -insert "CFBundleShortVersionString" -string "$(EPSILON_VERSION)" $@

# macOS uses icns files

.SECONDARY: $(SIMULATOR_ICONS) | $$(@D)/.

$(call simulator_app_resource,app.icns): $(SIMULATOR_ICONS) | $$(@D)/.
	$(call rule_label,ICNUTIL)
	$(Q) iconutil --convert icns --output $@ $(SIMULATOR_ICONSET)

simulator_app_deps += $(call simulator_app_resource,app.icns)

include ion/src/simulator/shared/apple/targets.mak

endif
