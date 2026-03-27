# Additional args to be passed to the executable like "-h" or "--language nl"
_args := $(ARGS)

# Run an executable file
$(OUTPUT_DIRECTORY)/%.bin.run: $(OUTPUT_DIRECTORY)/%.bin
	$^ $(_args)

$(call document_extension,bin.run,Execute <...>.bin)

# Open in the debugger
$(OUTPUT_DIRECTORY)/%.bin.debug: $(OUTPUT_DIRECTORY)/%.bin
	lldb -- $^

$(call document_extension,bin.debug,Load <...>.bin in a debugger)

# Run an executable file in the debugger
# Note: Using "-o r" prevents program interruption with ^C.
#       See https://github.com/llvm/llvm-project/issues/53673
$(OUTPUT_DIRECTORY)/%.bin.debug.run: $(OUTPUT_DIRECTORY)/%.bin
	lldb -o r -- $^ $(_args)

$(call document_extension,bin.debug.run, Load and execute <...>.bin in a debugger)

_simulator_app := $(OUTPUT_DIRECTORY)/%.app
_simulator_app_binary = $(_simulator_app)/Contents/MacOS/$(APP_NAME)
_simulator_app_plist = $(_simulator_app)/Contents/Info.plist
_simulator_app_resources_path = $(_simulator_app)/Contents/Resources

# We are using the APP_NAME and not the APP_FULL_NAME for the bundle name
# because it has to match the actual app directory name for the Finder to use
# the localized names. Case does not matter here.
$(_simulator_app_plist): $(PATH_haussmann)/data/Info.plist.$(PLATFORM) | $$(@D)/.
	$(call rule_label,PLUTIL)
	cp $< $@
	plutil -insert "CFBundleIdentifier" -string "$(APP_BUNDLE_ID)" $@
	plutil -insert "CFBundleName" -string "$(APP_NAME)" $@
	plutil -insert "CFBundleDisplayName" -string "$(APP_NAME)" $@
	plutil -insert "LSMinimumSystemVersion" -string "$(APPLE_PLATFORM_MIN_VERSION)" $@
	plutil -insert "CFBundleExecutable" -string "$(APP_NAME)" $@
	plutil -insert "CFBundleVersion" -string "$(APP_VERSION)" $@
	plutil -insert "CFBundleShortVersionString" -string "$(APP_VERSION)" $@

ifdef MACOS_PROVISIONING_PROFILE
$(_simulator_app_resources_path)/embedded.provisionprofile: $(MACOS_PROVISIONING_PROFILE) | $$(@D)/.
	$(call rule_label,COPY)
	cp $^ $@

$(OUTPUT_DIRECTORY)/app/entitlements.plist: $(MACOS_PROVISIONING_PROFILE)
	$(call rule_label,SCMS)
	security cms -D -i $(MACOS_PROVISIONING_PROFILE) | plutil -extract Entitlements xml1 - -o $@
	plutil -insert "com\.apple\.security\.app-sandbox" -bool true $@

_simulator_app_deps += $(OUTPUT_DIRECTORY)/app/entitlements.plist
_simulator_app_deps += $(_simulator_app_resources_path)/embedded.provisionprofile
else
$(_simulator_app_resources_path)/embedded.provisionprofile:
	$(warning Building without a provisioning profile. Please define MACOS_PROVISIONING_PROFILE to point to the .provisionprofile file you want to use.)
endif

include $(PATH_haussmann)/src/rules/shared.apple.mak

%.pkg: $(OUTPUT_DIRECTORY)/%.pkg
	@ :

MACOS_SIGNER_IDENTITY ?= Apple Distribution: NumWorks
MACOS_INSTALLER_IDENTITY ?= 3rd Party Mac Developer Installer: NumWorks
$(OUTPUT_DIRECTORY)/%.pkg: $(OUTPUT_DIRECTORY)/%.app
ifdef MACOS_PROVISIONING_PROFILE
	$(call rule_label,SIGN)
	codesign --force --options runtime --entitlements $(OUTPUT_DIRECTORY)/app/entitlements.plist --sign "$(MACOS_SIGNER_IDENTITY)" $(OUTPUT_DIRECTORY)/$*.app
	$(call rule_label,PRODUCT)
	productbuild --component $(OUTPUT_DIRECTORY)/$*.app /Applications $@ --identifier $(APP_BUNDLE_ID) --version $(APP_VERSION) --sign "$(MACOS_INSTALLER_IDENTITY)"
else
	$(warning Building without a provisioning profile. Please define MACOS_PROVISIONING_PROFILE to point to the .provisionprofile file you want to use.)
endif

# Run the simulator
%.app.run: $(_simulator_app)
	open $^ --args $(_args)

$(call document_extension,app.run,Start the simulator)

# Debug the simulator
%.app.debug: $(_simulator_app)
	lldb -- $^

$(call document_extension,app.debug,Debug the simulator)

# Run the simulator in the debugger
# Note: Using "-o r" prevents program interruption with ^C.
#       See https://github.com/llvm/llvm-project/issues/53673
%.app.debug.run: $(_simulator_app)
	lldb -o r -- $^ $(_args)

$(call document_extension,app.debug.run, Debug and execute the simulator)

# Reload the simulator
%.app.reload: $(_simulator_app)
	pgrep $(APP_NAME) && pkill -USR1 $(APP_NAME) || echo "No $(APP_NAME) executable running"
	cmd="open $^ --args --load-state-file /tmp/numworks.reload.nws $(_args)";\
	eval $$cmd || $$cmd

$(call document_extension,app.reload,Refresh a running simulator)
