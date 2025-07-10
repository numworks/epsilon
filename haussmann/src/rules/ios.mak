_simulator_app := $(OUTPUT_DIRECTORY)/%.app
_simulator_app_binary = $(_simulator_app)/$(APP_NAME)
_simulator_app_plist = $(_simulator_app)/Info.plist
_simulator_app_resources_path = $(_simulator_app)

# App resources

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
	$(Q) plutil -replace CFBundleIcons -json `plutil -extract CFBundleIcons json -o - $(OUTPUT_DIRECTORY)/app/assets/partial.plist` $@
	$(Q) plutil -replace CFBundleIcons~ipad -json `plutil -extract CFBundleIcons~ipad json -o - $(OUTPUT_DIRECTORY)/app/assets/partial.plist` $@


$(_simulator_app)/launch.storyboardc: ion/src/simulator/ios/launch.storyboard | $$(@D)/.
	$(call rule_label,IBTOOL)
	$(Q) $(IBTOOL) --minimum-deployment-target $(APPLE_PLATFORM_MIN_VERSION) --compile $@ $^

_simulator_app_deps += $(_simulator_app)/launch.storyboardc

include  $(PATH_haussmann)/src/rules/shared.apple.mak
