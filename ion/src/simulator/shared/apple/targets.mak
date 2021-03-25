.PRECIOUS: $(simulator_app_deps)

# CAUTION: The empty line in this rule is important!
# Indeed, rule without receipe serve a special purpose
# https://www.gnu.org/software/make/manual/html_node/Canceling-Rules.html
.PRECIOUS: $(BUILD_DIR)/%.app
$(BUILD_DIR)/%.app: $(simulator_app_deps)
	

# Standard usage: make -j8 PLATFORM=simulator TARGET=ios ACCEPT_OFFICIAL_TOS=1 IOS_PROVISIONNING_PROFILE=build/artifacts/NumWorks_Graphing_Calculator_Development.mobileprovision IOS_SIGNER_IDENTITY="Apple Development: [DEVELOPER NAME]" epsilon.official.ipa

IOS_SIGNER_IDENTITY ?= Apple Distribution: NumWorks
$(BUILD_DIR)/%.ipa: $(BUILD_DIR)/%.app
ifdef IOS_PROVISIONNING_PROFILE
	$(call rule_label,SIGN)
	$(Q) codesign --force --entitlements $(BUILD_DIR)/app/entitlements.plist --sign "$(IOS_SIGNER_IDENTITY)" $(BUILD_DIR)/$*.app
endif
	@ echo "$(shell printf "%-8s" "COPY")$(notdir $*).app into Payload"
	$(Q) cd $(dir $@) ; mkdir Payload; cp -r $(notdir $*).app Payload

	$(call rule_label,ZIP)
	$(Q) cd $(dir $@) ; zip -qr9 $(notdir $@) Payload
