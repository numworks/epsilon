$(OUTPUT_DIRECTORY)/%.bin: $(OUTPUT_DIRECTORY)/%.elf
	$(call rule_label,OBJCOPY)
	$(OBJCOPY) -O binary $< $@

$(call document_extension,bin,Extract plain binary from ELF file)

$(OUTPUT_DIRECTORY)/%.hex: $(OUTPUT_DIRECTORY)/%.elf
	$(call rule_label,OBJCOPY)
	$(OBJCOPY) -O ihex $< $@

$(call document_extension,hex,Extract Intel .hex from ELF file)

$(OUTPUT_DIRECTORY)/%.dfu: $(OUTPUT_DIRECTORY)/%.elf
	$(call rule_label,DFU)
	$(PYTHON) $(PATH_haussmann)/data/device/elf2dfu.py -i $< -o $@

$(call document_extension,dfu)

$(OUTPUT_DIRECTORY)/%.flash: $(OUTPUT_DIRECTORY)/%.dfu
	$(call rule_label,FLASH)
	$(PYTHON) $(PATH_haussmann)/data/device/dfu.py -D $< $(if $(DFULEAVE),-s $(DFULEAVE):leave)

$(call document_extension,flash,Write <...>.dfu to a device. Define DFULEAVE to jump at an address after the write.)
