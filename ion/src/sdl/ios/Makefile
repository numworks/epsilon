ion_src += $(addprefix ion/src/sdl/ios/, \
  images.m \
  language.m \
)

$(call object_for,ion/src/sdl/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

GOOGLE_ANALYTICS = 1
ifeq ($(GOOGLE_ANALYTICS),1)
ion_src += ion/src/sdl/ios/telemetry.m
$(call object_for,ion/src/sdl/ios/telemetry.m): SFLAGS += -Iion/src/sdl/ios/GoogleAnalyticsServices/GoogleAnalytics/Library

LDFLAGS += -lGoogleAnalyticsServices
LDFLAGS += -lsqlite3
LDFLAGS += -lz
LDFLAGS += -framework SystemConfiguration
LDFLAGS += -framework CoreData
LDFLAGS += -Lion/src/sdl/ios/GoogleAnalyticsServices
else
ion_src += ion/src/sdl/shared/telemetry_dummy.cpp
endif

# Cross-ARCH epsilon.bin

.PHONY: force_remake

$(BUILD_DIR)/%/epsilon.bin: force_remake
	$(Q) echo "MAKE    ARCH=$*"
	$(Q) $(MAKE) ARCH=$*

ifeq ($(IOS_PLATFORM),iphoneos)
ARCHS ?= arm64 armv7
UI_REQUIRED_CAPABILITIES += armv7
else ifeq ($(IOS_PLATFORM),iphonesimulator)
ARCHS ?= x86_64
endif

# iOS resources

ios_resource = $(addprefix $(BUILD_DIR)/app/Payload/Epsilon.app/,$(1))

$(call ios_resource,Epsilon): $(patsubst %,$(BUILD_DIR)/%/epsilon.bin,$(ARCHS)) | $$(@D)/.
	$(call rule_label,LIPO)
	$(Q) $(LIPO) -create $^ -output $@

$(call ios_resource,background.jpg): ion/src/sdl/assets/background.jpg | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $^ $@

$(call ios_resource,Info.plist): ion/src/sdl/ios/Info.plist $(call ios_resource,Assets.car)
	$(call rule_label,PLUTIL)
	$(Q) cp $< $@
	$(Q) plutil -insert "BuildMachineOSBuild" -string "$(IOS_BUILD_MACHINE_OS_BUILD)" $@
	$(Q) plutil -insert "MinimumOSVersion" -string "$(IOS_MIN_VERSION)" $@
	$(Q) plutil -insert "DTCompiler" -string "$(IOS_COMPILER)" $@
	$(Q) plutil -insert "DTPlatformBuild" -string "$(IOS_PLATFORM_BUILD)" $@
	$(Q) plutil -insert "DTPlatformName" -string "$(IOS_PLATFORM)" $@
	$(Q) plutil -insert "DTPlatformVersion" -string "$(IOS_PLATFORM_VERSION)" $@
	$(Q) plutil -insert "DTSDKName" -string "$(IOS_PLATFORM)$(IOS_PLATFORM_VERSION)" $@
	$(Q) plutil -insert "DTSDKBuild" -string "$(IOS_PLATFORM_BUILD)" $@
	$(Q) plutil -insert "DTXcode" -string "$(IOS_XCODE_VERSION)" $@
	$(Q) plutil -insert "DTXcodeBuild" -string "$(IOS_XCODE_BUILD)" $@
	$(Q) plutil -insert "CFBundleVersion" -string "$(EPSILON_VERSION)" $@
	$(Q) plutil -insert "CFBundleShortVersionString" -string "$(EPSILON_VERSION)" $@
	$(Q) $(foreach capability,$(UI_REQUIRED_CAPABILITIES),plutil -insert "UIRequiredDeviceCapabilities.0" -string "$(capability)" $@ ;)
	$(Q) plutil -replace CFBundleIcons -json `plutil -extract CFBundleIcons json -o - $(BUILD_DIR)/app/assets/partial.plist` $@
	$(Q) plutil -replace CFBundleIcons~ipad -json `plutil -extract CFBundleIcons~ipad json -o - $(BUILD_DIR)/app/assets/partial.plist` $@

$(call ios_resource,launch.storyboardc): ion/src/sdl/ios/launch.storyboard | $$(@D)/.
	$(call rule_label,IBTOOL)
	$(Q) $(IBTOOL) --minimum-deployment-target $(IOS_MIN_VERSION) --compile $@ $^

ifdef IOS_PROVISIONNING_PROFILE
$(call ios_resource,embedded.mobileprovision): $(IOS_PROVISIONNING_PROFILE) | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $^ $@
else
$(call ios_resource,embedded.mobileprovision):
	$(error Please define IOS_PROVISIONNING_PROFILE to point to the .mobileprovision file you want to use)
endif

$(BUILD_DIR)/app/entitlements.plist: $(call ios_resource,embedded.mobileprovision)
	$(call rule_label,SCMS)
	$(Q) security cms -D -i $(IOS_PROVISIONNING_PROFILE) | plutil -extract Entitlements xml1 - -o $@

asset_for = $(addprefix $(BUILD_DIR)/app/assets/Assets.xcassets/AppIcon.appiconset/,$(1))

icon_assets = $(addsuffix .png,$(addprefix icon_, \
  20x20 29x29 40x40 40x40 40x40 58x58 58x58 60x60 76x76 80x80 80x80 87x87 \
  120x120 120x120 152x152 167x167 180x180 1024x1024 \
))

$(call asset_for,Contents.json): ion/src/sdl/ios/icon_assets.json $(call asset_for,$(icon_assets)) | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $< $@

$(call asset_for,icon_%.png): ion/src/sdl/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background "#FFB734" -resize $* $< $@

$(call ios_resource,Assets.car): $(call asset_for,Contents.json) | $$(@D)/.
	$(call rule_label,ACTOOL)
	$(Q) $(ACTOOL) --compile $(BUILD_DIR)/app/Payload/Epsilon.app --minimum-deployment-target $(IOS_MIN_VERSION) --platform $(IOS_PLATFORM) --app-icon AppIcon --output-partial-info-plist $(BUILD_DIR)/app/assets/partial.plist $(BUILD_DIR)/app/assets/Assets.xcassets > /dev/null

epsilon_app_deps = $(call ios_resource,\
  Assets.car \
  Epsilon \
  Info.plist \
  background.jpg \
  launch.storyboardc \
)

$(BUILD_DIR)/app/epsilon.ipa: $(epsilon_app_deps) $(BUILD_DIR)/app/entitlements.plist
	$(call rule_label,SIGN)
	$(Q) codesign --force --entitlements $(BUILD_DIR)/app/entitlements.plist --sign "iPhone Distribution: NumWorks" $(BUILD_DIR)/app/Payload/Epsilon.app
	$(call rule_label,ZIP)
	$(Q) cd $(dir $@) ; zip -qr9 $(notdir $@) Payload

.PHONY: epsilon.ipa
epsilon.ipa: $(BUILD_DIR)/app/epsilon.ipa

ifeq ($(IOS_PLATFORM),iphonesimulator)
.PHONY: epsilon_run
epsilon_run: $(epsilon_app_deps)
	xcrun simctl install booted $(BUILD_DIR)/app/Payload/Epsilon.app
endif
