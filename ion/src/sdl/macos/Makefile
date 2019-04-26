src += $(addprefix ion/src/sdl/macos/, \
  images.m \
)

src += $(addprefix ion/src/sdl/shared/, \
  apple/language.m \
  telemetry_dummy.cpp \
)

# App resources

SIMULATOR_ICON_SIZES = 16x16 32x32 64x64 128x128 256x256 512x512 1024x1024
SIMULATOR_APP_BINARY_PATH = Contents/MacOS/
SIMULATOR_APP_RESOURCE_PATH = Contents/Resources/
SIMULATOR_APP_PLIST_PATH = Contents/

include ion/src/sdl/shared/apple/Makefile

$(call simulator_app_plist,Info.plist): ion/src/sdl/macos/Info.plist
	$(call rule_label,PLUTIL)
	$(Q) cp $< $@
	$(Q) plutil -insert "LSMinimumSystemVersion" -string "$(MACOS_MIN_VERSION)" $@
	$(Q) plutil -insert "CFBundleVersion" -string "$(EPSILON_VERSION)" $@
	$(Q) plutil -insert "CFBundleShortVersionString" -string "$(EPSILON_VERSION)" $@

# macOS uses icns files

$(call simulator_app_resource,app.icns): $(SIMULATOR_ICONS) | $$(@D)/.
	$(call rule_label,ICONUTIL)
	$(Q) iconutil --convert icns --output $@ $(SIMULATOR_ICONSET)

SIMULATOR_APP_DEPS += $(call simulator_app_resource,app.icns)

$(BUILD_DIR)/app/Epsilon.app: $(SIMULATOR_APP_DEPS)
