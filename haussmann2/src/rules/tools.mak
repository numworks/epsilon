$(TOOLS_DIRECTORY)/%: $$(foreach m,$$(basename $$(call libraries_for_flavored_goal,%)),$$(call _tool_objects_for_sources,$$(call sources_for_flavored_module,$$m))) | $$(@D)/.
	$(call rule_label,HOSTLD)
	$(QUIET) $(HOSTLD) $(TOOLS_SFLAGS) $^ $(TOOLS_LDFLAGS) -o $@

$(TOOLS_DIRECTORY)/%.o: %.c | $$(@D)/.
	$(call rule_label,HOSTCC)
	$(QUIET) $(HOSTCC) $(TOOLS_SFLAGS) $(TOOLS_CFLAGS) -c $< -o $@

$(TOOLS_DIRECTORY)/%.o: %.cpp | $$(@D)/.
	$(call rule_label,HOSTCXX)
	$(QUIET) $(HOSTCXX) $(TOOLS_SFLAGS) $(TOOLS_CXXFLAGS) -c $< -o $@

# Helpers

define _tool_objects_for_sources
$(addprefix $(TOOLS_DIRECTORY)/,$(addsuffix .o,$(basename $1)))
endef
