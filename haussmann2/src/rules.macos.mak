# Run an executable file
$(OUTPUT_DIRECTORY)/%.run: $(OUTPUT_DIRECTORY)/%.bin
	$(QUIET) $^
