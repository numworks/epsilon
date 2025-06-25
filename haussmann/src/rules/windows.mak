# Run an executable file
$(OUTPUT_DIRECTORY)/%.bin.run: $(OUTPUT_DIRECTORY)/%.$(EXECUTABLE_EXTENSION)
	./$^

$(call document_extension,run,Execute <...>.exe)
