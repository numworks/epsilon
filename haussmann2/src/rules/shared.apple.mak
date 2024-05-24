# Additional object files
$(OUTPUT_DIRECTORY)/%.o: $$(call strip_arch_dir,%).m | $$(@D)/.
	$(call rule_label,OCC)
	$(QUIET) $(CC) $(PRIORITY_SFLAGS) $(SFLAGS) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIRECTORY)/%.o: $$(call strip_arch_dir,%).mm | $$(@D)/.
	$(call rule_label,OCC)
	$(QUIET) $(CXX) $(PRIORITY_SFLAGS) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

# Create a packaged app, made of:
# - an executable grouping the binaries for all supported archs
# - the Info.plist file
# - various resources
$(eval $(call document_extension,app))

_simulator_app := $(OUTPUT_DIRECTORY)/%.app

%.app: $(_simulator_app)
	@ :

$(call assert_defined,_simulator_app_binary)
$(call assert_defined,_simulator_app_plist)

$(_simulator_app): $(_simulator_app_binary) $(_simulator_app_plist) $(_simulator_app_resources) | $$(@D)/.
	@ :

$(_simulator_app_binary): $(call target_foreach_arch,%.$(EXECUTABLE_EXTENSION)) | $$(@D)/.
	$(call rule_label,LIPO)
	$(QUIET) $(LIPO) -create $^ -output $@
