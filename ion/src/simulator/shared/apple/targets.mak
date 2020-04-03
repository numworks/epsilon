.PRECIOUS: $(simulator_app_deps)

# CAUTION: The empty line in this rule is important!
# Indeed, rule without receipe serve a special purpose
# https://www.gnu.org/software/make/manual/html_node/Canceling-Rules.html
.PRECIOUS: $(BUILD_DIR)/%.app
$(BUILD_DIR)/%.app: $(simulator_app_deps)
	

$(BUILD_DIR)/%.ipa: $(BUILD_DIR)/%.app
ifdef IOS_PROVISIONNING_PROFILE
	$(call rule_label,SIGN)
	$(Q) codesign --force --entitlements $(BUILD_DIR)/app/entitlements.plist --sign "Apple Distribution: NumWorks" $(BUILD_DIR)/$*.app
endif
	$(call rule_label,ZIP)
	$(Q) cd $(dir $@) ; zip -qr9 $(notdir $@) $*.app
