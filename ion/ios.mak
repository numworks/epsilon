SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  shared/apple/platform_images.mm \
  shared/apple/platform_language.mm \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/keyboard_callback.cpp \
  shared/dummy/window_callback.cpp \
  shared/haptics.cpp \
  shared/circuit_breaker.cpp \
  shared/clipboard_helper_sdl.cpp \
  shared/dummy/journal.cpp \
)

# TODO collect_registers.cpp depends on arch, tweak haussmann
SOURCES_ion += $(PATH_ion)/src/shared/collect_registers.cpp

_ion_simulator_files := 0

_ion_simulator_assets_path := $(OUTPUT_DIRECTORY)/app/assets/Assets.xcassets
_ion_simulator_icons_sizes := 20x20 29x29 40x40 58x58 60x60 76x76 \
  80x80 87x87 120x120 152x152 167x167 180x180 1024x1024
_ion_simulator_iconset := $(_ion_simulator_assets_path)/AppIcon.appiconset

include $(PATH_ion)/shared.apple.mak

$(_ion_simulator_iconset)/Contents.json: ion/src/simulator/ios/icon_assets.json $(_ion_simulator_icons) | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $< $@

$(_simulator_app_resources_path)/Assets.car: $(_ion_simulator_iconset)/Contents.json | $$(@D)/.
	$(call rule_label,ACTOOL)
	$(Q) $(ACTOOL) --compile $(OUTPUT_DIRECTORY)/$*.app --minimum-deployment-target $(APPLE_PLATFORM_MIN_VERSION) --platform $(APPLE_SDK) --app-icon AppIcon --output-partial-info-plist $(OUTPUT_DIRECTORY)/app/assets/partial.plist $(_ion_simulator_assets_path) > /dev/null
