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

$(call ios_resource,icon.png):: ion/src/sdl/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background "#FFB734" $< $@

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

$(call ios_resource,Info.plist): ion/src/sdl/ios/Info.plist
	$(call rule_label,PLUTIL)
	$(Q) cp $^ $@
	$(Q) plutil -insert "DTPlatformName" -string "$(SDK)" $@
	$(foreach arch,$(ARCHS),$(Q) plutil -insert "UIRequiredDeviceCapabilities.0" -string "$(arch)" $@)

$(call ios_resource,launch.storyboardc): ion/src/sdl/ios/launch.storyboard
	$(call rule_label,IBTOOL)
	$(Q) ibtool --compile $@ $^

$(call ios_resource,embedded.mobileprovision): $(IOS_PROVISIONNING_PROFILE)
	$(call rule_label,COPY)
	$(Q) cp $^ $@

epsion_ipa_deps: $(call ios_resource,\
  Epsilon.signed \
  Info.plist \
  background.jpg \
  embedded.mobileprovision \
  icon.png \
  launch.storyboardc \
)

$(BUILD_DIR)/app/epsilon.ipa: epsion_ipa_deps
	$(call rule_label,ZIP)
	$(Q) cd $(dir $@) ; zip -qr9 $(notdir $@) Payload

epsilon_run: $(BUILD_DIR)/app/Epsilon.app
	xcrun simctl install booted $(BUILD_DIR)/app/Epsilon.app
