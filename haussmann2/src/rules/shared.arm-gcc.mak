$(OUTPUT_DIRECTORY)/%.bin: $(OUTPUT_DIRECTORY)/%.elf
	$(call rule_label,OBJCOPY)
	$(QUIET) $(OBJCOPY) -O binary $< $@

$(call document_extension,bin,Extract plain binary from ELF file)
