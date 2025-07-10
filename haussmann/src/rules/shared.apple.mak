# Additional object files
$(call rule_for_object, \
  OCC, m, \
  $$(CC) $$(PRIORITY_SFLAGS) $$(SFLAGS) $$(CFLAGS) -c $$< -o $$@ \
)

$(call rule_for_object, \
  OCC, mm, \
  $$(CXX) $$(PRIORITY_SFLAGS) $$(SFLAGS) $$(CXXFLAGS) -c $$< -o $$@ \
)

$(call assert_defined,_simulator_app)
$(call assert_defined,_simulator_app_binary)
$(call assert_defined,_simulator_app_plist)

# Create a packaged app, made of:
# - an executable grouping the binaries for all supported archs
# - the Info.plist file
# - various resources
$(call document_extension,app)

%.app: $(_simulator_app)
	@ :

_simulator_app_deps += $(_simulator_app_plist) $$(addprefix $(_simulator_app_resources_path)/,$$(_simulator_app_resources)) $(_simulator_app_binary)

$(_simulator_app): $(_simulator_app_deps) | $$(@D)/.
	@ :

$(_simulator_app_binary): $(call all_targets_named,%.$(EXECUTABLE_EXTENSION)) | $$(@D)/.
	$(call rule_label,LIPO)
	$(LIPO) -create $^ -output $@

# rule_for_simulator_resource, <label>, <targets>, <prerequisites>, <recipe>
define rule_for_simulator_resource
$(eval \
_simulator_app_resources += $(strip $2)
$(addprefix $(_simulator_app_resources_path)/,$(strip $2)): $(strip $3) | $$$$(@D)/.
	$$(call rule_label,$1)
	$4
)
endef

%.ipa: $(OUTPUT_DIRECTORY)/%.ipa
	@ :

IOS_SIGNER_IDENTITY ?= Apple Distribution: NumWorks
$(OUTPUT_DIRECTORY)/%.ipa: $(OUTPUT_DIRECTORY)/%.app
ifdef IOS_PROVISIONNING_PROFILE
	$(call rule_label,SIGN)
	$(Q) codesign --force --entitlements $(OUTPUT_DIRECTORY)/app/entitlements.plist --sign "$(IOS_SIGNER_IDENTITY)" $(OUTPUT_DIRECTORY)/$*.app
endif
	@ echo "$(shell printf "%-8s" "COPY")$(notdir $*).app into Payload"
	$(Q) cd $(dir $@) ; mkdir Payload; cp -r $(notdir $*).app Payload

	$(call rule_label,ZIP)
	$(Q) cd $(dir $@) ; zip -qr9 $(notdir $@) Payload
