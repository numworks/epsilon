# Run an executable file
$(OUTPUT_DIRECTORY)/%.run: $(OUTPUT_DIRECTORY)/%.bin
	$(QUIET) $^

$(eval $(call document_extension,run,Execute <...>.bin))

include $(PATH_haussmann)/src/rules/shared.apple.mak
