$(OUTPUT_DIRECTORY)/%.bin: $(OUTPUT_DIRECTORY)/%.elf
	$(QUIET) echo "OBJCOPY\t$@"
	$(QUIET) $(OBJCOPY) -O binary $< $@

$(eval $(call document_extension,bin,Extract plain binary from ELF file))
