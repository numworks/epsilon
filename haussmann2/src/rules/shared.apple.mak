# Additional object files
$(OUTPUT_DIRECTORY)/%.o: $$(call strip_arch_dir,%).m | $$(@D)/.
	$(call rule_label,OCC)
	$(QUIET) $(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIRECTORY)/%.o: $$(call strip_arch_dir,%).mm | $$(@D)/.
	$(call rule_label,OCC)
	$(QUIET) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@
