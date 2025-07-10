_simulator_app := $(OUTPUT_DIRECTORY)/%.app
_simulator_app_binary = $(_simulator_app)/$(APP_NAME)
_simulator_app_plist = $(_simulator_app)/Info.plist
_simulator_app_resources_path = $(_simulator_app)

# App resources

# TODO_PAULINE factorize paths with ion
_ion_simulator_icons_sizes := 20x20 29x29 40x40 40x40 40x40 58x58 58x58 60x60 76x76 \
  80x80 80x80 87x87 120x120 120x120 152x152 167x167 180x180 1024x1024
_ion_simulator_assets_path := $(OUTPUT_DIRECTORY)/app/assets/Assets.xcassets
_ion_simulator_iconset := $(_ion_simulator_assets_path)/AppIcon.appiconset
_ion_simulator_icons := $(patsubst %,$(_ion_simulator_iconset)/icon_%.png,$(_ion_simulator_icons_sizes))

$(_ion_simulator_iconset)/Contents.json: ion/src/simulator/ios/icon_assets.json $(_ion_simulator_icons) | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $< $@

$(_simulator_app_resources_path)/Assets.car: $(_ion_simulator_iconset)/Contents.json | $$(@D)/.
	$(call rule_label,ACTOOL)
	$(Q) $(ACTOOL) --compile $(OUTPUT_DIRECTORY)/$*.app --minimum-deployment-target $(APPLE_PLATFORM_MIN_VERSION) --platform $(APPLE_SDK) --app-icon AppIcon --output-partial-info-plist $(OUTPUT_DIRECTORY)/app/assets/partial.plist $(_ion_simulator_assets_path) > /dev/null

$(_simulator_app_plist): $(PATH_haussmann)/data/Info.plist.$(PLATFORM) $(_simulator_app_resources_path)/Assets.car | $$(@D)/. 
	$(call rule_label,PLUTIL)
	$(Q) cp $< $@
	$(Q) plutil -insert "BuildMachineOSBuild" -string "$(IOS_BUILD_MACHINE_OS_BUILD)" $@
	$(Q) plutil -insert "MinimumOSVersion" -string "$(APPLE_PLATFORM_MIN_VERSION)" $@
	$(Q) plutil -insert "DTCompiler" -string "$(IOS_COMPILER)" $@
	$(Q) plutil -insert "DTPlatformBuild" -string "$(IOS_PLATFORM_BUILD)" $@
	$(Q) plutil -insert "DTPlatformName" -string "$(APPLE_SDK)" $@
	$(Q) plutil -insert "DTPlatformVersion" -string "$(IOS_PLATFORM_VERSION)" $@
	$(Q) plutil -insert "DTSDKName" -string "$(APPLE_SDK)$(IOS_PLATFORM_VERSION)" $@
	$(Q) plutil -insert "DTSDKBuild" -string "$(IOS_PLATFORM_BUILD)" $@
	$(Q) plutil -insert "DTXcode" -string "$(IOS_XCODE_VERSION)" $@
	$(Q) plutil -insert "DTXcodeBuild" -string "$(IOS_XCODE_BUILD)" $@
	$(Q) plutil -insert "CFBundleVersion" -string "$(EPSILON_VERSION)" $@
	$(Q) plutil -insert "CFBundleShortVersionString" -string "$(EPSILON_VERSION)" $@
	$(Q) $(foreach capability,$(UI_REQUIRED_CAPABILITIES),plutil -insert "UIRequiredDeviceCapabilities.0" -string "$(capability)" $@ ;)
	$(Q) plutil -replace CFBundleIcons -json `plutil -extract CFBundleIcons json -o - $(OUTPUT_DIRECTORY)/app/assets/partial.plist` $@
	$(Q) plutil -replace CFBundleIcons~ipad -json `plutil -extract CFBundleIcons~ipad json -o - $(OUTPUT_DIRECTORY)/app/assets/partial.plist` $@


$(_simulator_app)/launch.storyboardc: ion/src/simulator/ios/launch.storyboard | $$(@D)/.
	$(call rule_label,IBTOOL)
	$(Q) $(IBTOOL) --minimum-deployment-target $(APPLE_PLATFORM_MIN_VERSION) --compile $@ $^

_simulator_app_deps += $(_simulator_app)/launch.storyboardc

include  $(PATH_haussmann)/src/rules/shared.apple.mak
