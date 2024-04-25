# Run an executable file
$(OUTPUT_DIRECTORY)/%.run: $(OUTPUT_DIRECTORY)/%.bin
	$(QUIET) $^

# Additional object files
$(OUTPUT_DIRECTORY)/%.o: %.m | $$(@D)/.
	$(QUIET) echo "OCC\t$@"
	$(QUIET) $(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@
