$(OUTPUT_DIRECTORY)/%.bin: $(OUTPUT_DIRECTORY)/%.elf
	$(call rule_label,OBJCOPY)
	$(QUIET) $(OBJCOPY) -O binary $< $@

$(call document_extension,bin,Extract plain binary from ELF file)

$(OUTPUT_DIRECTORY)/%.dfu: $(OUTPUT_DIRECTORY)/%.elf
	$(call rule_label,DFU)
	$(QUIET) $(PYTHON) $(PATH_haussmann)/data/device/elf2dfu.py -i $< -o $@

$(call document_extension,dfu)

$(OUTPUT_DIRECTORY)/%.flash: $(OUTPUT_DIRECTORY)/%.dfu
	$(call rule_label,FLASH)
	$(QUIET) $(PYTHON) $(PATH_haussmann)/data/device/dfu.py -D $< $(if $(DFULEAVE),-s $(DFULEAVE):leave)
