src += $(addprefix ion/src/sdl/ios/, \
  images.m \
)

$(call object_for,ion/src/sdl/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

# Cross-ARCH epsilon.bin

.PHONY: force_remake

$(BUILD_DIR)/%/epsilon.bin: force_remake
	$(Q) echo "MAKE    ARCH=$*"
	$(Q) $(MAKE) ARCH=$*

ARCHS ?= x86_64

# iOS resources

ios_resource = $(addprefix $(BUILD_DIR)/app/Payload/Epsilon.app/,$(1))

$(BUILD_DIR)/app/entitlements.plist: ion/src/sdl/ios/entitlements.plist
	$(call rule_label,PLUTIL)
	$(Q) cp $^ $@
	$(Q) plutil -insert "com\.apple\.developer\.team-identifier" -string "$(IOS_IDENTIFIER)" $@
	$(Q) plutil -insert "application-identifier" -string "$(IOS_IDENTIFIER).com.numworks.calculator" $@
	$(Q) plutil -insert "keychain-access-groups.0" -string "$(IOS_IDENTIFIER).com.numworks.calculator" $@

$(call ios_resource,Epsilon): $(patsubst %,$(BUILD_DIR)/%/epsilon.bin,$(ARCHS)) | $$(@D)/.
	$(call rule_label,LIPO)
	$(Q) lipo -create $^ -output $@

.PHONY: $(call ios_resource,Epsilon).signed
$(call ios_resource,Epsilon).signed: $(call ios_resource,Epsilon) $(BUILD_DIR)/app/entitlements.plist
	$(call rule_label,SIGN)
	$(Q) codesign --force --entitlements $(BUILD_DIR)/app/entitlements.plist --sign "iPhone Distribution: NumWorks" $<

$(call ios_resource,background.jpg): ion/src/sdl/assets/background.jpg
	$(call rule_label,COPY)
	$(Q) cp $^ $@

$(call ios_resource,Info.plist): ion/src/sdl/ios/Info.plist $(call ios_resource,Assets.car)
	$(call rule_label,PLUTIL)
	$(Q) cp $< $@
	$(Q) plutil -insert "MinimumOSVersion" -string "$(IOS_MIN_VERSION)" $@
	$(Q) plutil -insert "DTPlatformName" -string "$(IOS_PLATFORM)" $@
	$(Q) plutil -insert "CFBundleVersion" -string "$(EPSILON_VERSION).$(PATCH_LEVEL)" $@
	$(Q) plutil -insert "CFBundleShortVersionString" -string "$(EPSILON_VERSION)" $@
	$(foreach arch,$(ARCHS),$(Q) plutil -insert "UIRequiredDeviceCapabilities.0" -string "$(arch)" $@)
	$(Q) plutil -replace CFBundleIcons -json `plutil -extract CFBundleIcons json -o - $(BUILD_DIR)/app/assets/partial.plist` $@
	$(Q) plutil -replace CFBundleIcons~ipad -json `plutil -extract CFBundleIcons~ipad json -o - $(BUILD_DIR)/app/assets/partial.plist` $@

$(call ios_resource,launch.storyboardc): ion/src/sdl/ios/launch.storyboard
	$(call rule_label,IBTOOL)
	$(Q) ibtool --compile $@ $^

$(call ios_resource,embedded.mobileprovision): $(IOS_PROVISIONNING_PROFILE)
	$(call rule_label,COPY)
	$(Q) cp $^ $@

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

$(call ios_resource,Assets.car): $(call asset_for,Contents.json)
	$(call rule_label,ACTOOL)
	$(Q) actool --compile $(BUILD_DIR)/app/Payload/Epsilon.app --minimum-deployment-target $(IOS_MIN_VERSION) --platform $(IOS_PLATFORM) --app-icon AppIcon --output-partial-info-plist $(BUILD_DIR)/app/assets/partial.plist $(BUILD_DIR)/app/assets/Assets.xcassets

epsilon_ipa_deps: $(call ios_resource,\
  Assets.car \
  Epsilon.signed \
  Info.plist \
  background.jpg \
  embedded.mobileprovision \
  launch.storyboardc \
)

$(epsilon_ipa_deps): | $$(@D)/.

$(BUILD_DIR)/app/epsilon.ipa: epsilon_ipa_deps
	$(call rule_label,ZIP)
	$(Q) cd $(dir $@) ; zip -qr9 $(notdir $@) Payload

epsilon_run: $(BUILD_DIR)/app/Epsilon.app
	xcrun simctl install booted $(BUILD_DIR)/app/Epsilon.app
