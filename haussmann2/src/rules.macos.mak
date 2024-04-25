# Run an executable file
$(OUTPUT_DIRECTORY)/%.run: $(OUTPUT_DIRECTORY)/%.bin
	$(QUIET) $^

$(eval $(call document_extension,run,Execute <...>.bin))

# Additional object files
$(OUTPUT_DIRECTORY)/%.o: %.m | $$(@D)/.
	$(QUIET) echo "OCC\t$@"
	$(QUIET) $(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@
