# Run an executable file
$(OUTPUT_DIRECTORY)/%.run: $(OUTPUT_DIRECTORY)/%.bin
	$^

$(call document_extension,run,Execute <...>.bin)

_simulator_app_binary = $(_simulator_app)/Contents/MacOS/$(APP_NAME)
_simulator_app_plist = $(_simulator_app)/Contents/Info.plist
_simulator_app_resources_path = $(_simulator_app)/Contents/Resources

include $(PATH_haussmann)/src/rules/shared.apple.mak

# Run the simulator
.PHONY: %.app.run
%.app.run: $(_simulator_app)
	open $^

$(call document_extension,app.run,Start the simulator)

$(_simulator_app_plist): $(PATH_haussmann)/data/Info.plist.$(PLATFORM) | $$(@D)/.
	$(call rule_label,PLUTIL)
	cp $< $@
	plutil -insert "LSMinimumSystemVersion" -string "$(APPLE_PLATFORM_MIN_VERSION)" $@
	plutil -insert "CFBundleExecutable" -string "$(APP_NAME)" $@
	plutil -insert "CFBundleVersion" -string "$(APP_VERSION)" $@
	plutil -insert "CFBundleShortVersionString" -string "$(APP_VERSION)" $@

