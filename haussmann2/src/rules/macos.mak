# Run an executable file
$(OUTPUT_DIRECTORY)/%.run: $(OUTPUT_DIRECTORY)/%.bin
	$(QUIET) $^

$(call document_extension,run,Execute <...>.bin)

_simulator_app_binary = $(_simulator_app)/Contents/MacOS/$(APP_NAME)
_simulator_app_plist = $(_simulator_app)/Contents/Info.plist
_simulator_app_resources_path = $(_simulator_app)/Contents/Resources

include $(PATH_haussmann)/src/rules/shared.apple.mak

$(_simulator_app_plist): $(PATH_haussmann)/data/Info.plist.$(PLATFORM)
	$(call rule_label,PLUTIL)
	$(QUIET) cp $< $@
	$(QUIET) plutil -insert "LSMinimumSystemVersion" -string "$(APPLE_PLATFORM_MIN_VERSION)" $@
	$(QUIET) plutil -insert "CFBundleExecutable" -string "$(APP_NAME)" $@
	$(QUIET) plutil -insert "CFBundleVersion" -string "$(APP_VERSION)" $@
	$(QUIET) plutil -insert "CFBundleShortVersionString" -string "$(APP_VERSION)" $@

