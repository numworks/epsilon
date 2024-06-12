# Additional object files
$(call rule_for_object, \
  OCC, m, \
  $$(CC) $$(PRIORITY_SFLAGS) $$(SFLAGS) $$(CFLAGS) -c $$< -o $$@ \
)

$(call rule_for_object, \
  OCC, mm, \
  $$(CXX) $$(PRIORITY_SFLAGS) $$(SFLAGS) $$(CXXFLAGS) -c $$< -o $$@ \
)

# Create a packaged app, made of:
# - an executable grouping the binaries for all supported archs
# - the Info.plist file
# - various resources
$(call document_extension,app)

_simulator_app := $(OUTPUT_DIRECTORY)/%.app

%.app: $(_simulator_app)
	@ :

$(call assert_defined,_simulator_app_binary)
$(call assert_defined,_simulator_app_plist)

$(_simulator_app): $(_simulator_app_plist) $$(addprefix $(_simulator_app_resources_path)/,$$(_simulator_app_resources)) $(_simulator_app_binary) | $$(@D)/.
	@ :

$(_simulator_app_binary): $(call all_targets_named,%.$(EXECUTABLE_EXTENSION)) | $$(@D)/.
	$(call rule_label,LIPO)
	$(QUIET) $(LIPO) -create $^ -output $@

# rule_for_simulator_resource, <label>, <targets>, <prerequisites>, <recipe>
define rule_for_simulator_resource
$(eval \
_simulator_app_resources += $(strip $2)
$(addprefix $(_simulator_app_resources_path)/,$(strip $2)): $(strip $3) | $$$$(@D)/.
	$$(call rule_label,$1)
	$(QUIET) $4
)
endef
