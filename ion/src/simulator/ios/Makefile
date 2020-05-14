ion_src += $(addprefix ion/src/simulator/ios/, \
  images.m \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  apple/language.m \
  dummy/callback.cpp \
)

ion_src += ion/src/shared/collect_registers.cpp

$(call object_for,ion/src/simulator/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

ifeq ($(EPSILON_TELEMETRY),1)
# Display a nice error if the Google Analytics SDK is not installed
$(call object_for,ion/src/simulator/ios/telemetry.m): ion/src/simulator/ios/GoogleAnalyticsServices/GoogleAnalytics/Library/GAI.h
ion/src/simulator/ios/GoogleAnalyticsServices/GoogleAnalytics/Library/GAI.h:
	$(error GoogleAnalyticsServices needs to be installed in ion/src/simulator/ios/GoogleAnalyticsServices)

ion_src += ion/src/simulator/ios/telemetry.m
$(call object_for,ion/src/simulator/ios/telemetry.m): SFLAGS += -Iion/src/simulator/ios/GoogleAnalyticsServices/GoogleAnalytics/Library

LDFLAGS += -lGoogleAnalyticsServices
LDFLAGS += -lsqlite3
LDFLAGS += -lz
LDFLAGS += -framework SystemConfiguration
LDFLAGS += -framework CoreData
LDFLAGS += -Lion/src/simulator/ios/GoogleAnalyticsServices
endif

ifndef ARCH

# App resources

SIMULATOR_ICON_SIZES = 20x20 29x29 40x40 40x40 40x40 58x58 58x58 60x60 76x76 \
  80x80 80x80 87x87 120x120 120x120 152x152 167x167 180x180 1024x1024
SIMULATOR_ASSETS_PATH = $(BUILD_DIR)/app/assets/Assets.xcassets
SIMULATOR_ICONSET = $(SIMULATOR_ASSETS_PATH)/AppIcon.appiconset

include ion/src/simulator/shared/apple/helpers.mak

$(call simulator_app_plist,Info.plist): ion/src/simulator/ios/Info.plist $(call simulator_app_resource,Assets.car)
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
	$(Q) plutil -replace CFBundleIcons -json `plutil -extract CFBundleIcons json -o - $(BUILD_DIR)/app/assets/partial.plist` $@
	$(Q) plutil -replace CFBundleIcons~ipad -json `plutil -extract CFBundleIcons~ipad json -o - $(BUILD_DIR)/app/assets/partial.plist` $@

$(call simulator_app_resource,launch.storyboardc): ion/src/simulator/ios/launch.storyboard | $$(@D)/.
	$(call rule_label,IBTOOL)
	$(Q) $(IBTOOL) --minimum-deployment-target $(APPLE_PLATFORM_MIN_VERSION) --compile $@ $^

ifdef IOS_PROVISIONNING_PROFILE
$(call simulator_app_resource,embedded.mobileprovision): $(IOS_PROVISIONNING_PROFILE) | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $^ $@

$(BUILD_DIR)/app/entitlements.plist: $(IOS_PROVISIONNING_PROFILE)
	$(call rule_label,SCMS)
	$(Q) security cms -D -i $(IOS_PROVISIONNING_PROFILE) | plutil -extract Entitlements xml1 - -o $@

simulator_app_deps += $(BUILD_DIR)/app/entitlements.plist
simulator_app_deps += $(call simulator_app_resource,embedded.mobileprovision)
else
$(call simulator_app_resource,embedded.mobileprovision):
	$(warning Building without a provisionning profile. Please define IOS_PROVISIONNING_PROFILE to point to the .mobileprovision file you want to use.)
endif

$(SIMULATOR_ICONSET)/Contents.json: ion/src/simulator/ios/icon_assets.json $(SIMULATOR_ICONS) | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $< $@

$(call simulator_app_resource,Assets.car): $(SIMULATOR_ICONSET)/Contents.json | $$(@D)/.
	$(call rule_label,ACTOOL)
	$(Q) $(ACTOOL) --compile $(BUILD_DIR)/$*.app --minimum-deployment-target $(APPLE_PLATFORM_MIN_VERSION) --platform $(APPLE_SDK) --app-icon AppIcon --output-partial-info-plist $(BUILD_DIR)/app/assets/partial.plist $(SIMULATOR_ASSETS_PATH) > /dev/null

simulator_app_deps += $(call simulator_app_resource,Assets.car)
simulator_app_deps += $(call simulator_app_resource,launch.storyboardc)

include ion/src/simulator/shared/apple/targets.mak

endif
