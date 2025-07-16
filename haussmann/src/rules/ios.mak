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
	$(Q) plutil -insert "CFBundleExecutable" -string "$(APP_NAME)" $@
	$(Q) plutil -insert "CFBundleVersion" -string "$(APP_VERSION)" $@
	$(Q) plutil -insert "CFBundleShortVersionString" -string "$(APP_VERSION)" $@
	$(Q) plutil -replace CFBundleIcons -json `plutil -extract CFBundleIcons json -o - $(OUTPUT_DIRECTORY)/app/assets/partial.plist` $@
	$(Q) plutil -replace CFBundleIcons~ipad -json `plutil -extract CFBundleIcons~ipad json -o - $(OUTPUT_DIRECTORY)/app/assets/partial.plist` $@


$(_simulator_app_resources_path)/launch.storyboardc: ion/src/simulator/ios/launch.storyboard | $$(@D)/.
	$(call rule_label,IBTOOL)
	$(Q) $(IBTOOL) --minimum-deployment-target $(APPLE_PLATFORM_MIN_VERSION) --compile $@ $^

_simulator_app_deps += $(_simulator_app_resources_path)/launch.storyboardc

ifdef IOS_PROVISIONNING_PROFILE
$(_simulator_app_resources_path)/embedded.mobileprovision: $(IOS_PROVISIONNING_PROFILE) | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $^ $@

$(OUTPUT_DIRECTORY)/app/entitlements.plist: $(IOS_PROVISIONNING_PROFILE)
	$(call rule_label,SCMS)
	$(Q) security cms -D -i $(IOS_PROVISIONNING_PROFILE) | plutil -extract Entitlements xml1 - -o $@

_simulator_app_deps += $(OUTPUT_DIRECTORY)/app/entitlements.plist
_simulator_app_deps += $(_simulator_app_resources_path)/embedded.mobileprovision
else
$(_simulator_app_resources_path)/embedded.mobileprovision:
	$(warning Building without a provisionning profile. Please define IOS_PROVISIONNING_PROFILE to point to the .mobileprovision file you want to use.)
endif

include  $(PATH_haussmann)/src/rules/shared.apple.mak
