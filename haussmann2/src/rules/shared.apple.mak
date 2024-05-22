# Additional object files
$(OUTPUT_DIRECTORY)/%.o: $$(call strip_arch_dir,%).m | $$(@D)/.
	$(QUIET) echo "OCC\t$@"
	$(QUIET) $(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIRECTORY)/%.o: $$(call strip_arch_dir,%).mm | $$(@D)/.
	$(QUIET) echo "OCC\t$@"
	$(QUIET) $(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@
