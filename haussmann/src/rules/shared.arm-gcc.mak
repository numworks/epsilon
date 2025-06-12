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

$(OUTPUT_DIRECTORY)/%.st_flash: $(OUTPUT_DIRECTORY)/%.elf
	$(call rule_label,STFLASH)
	STM32_Programmer_CLI -c port=SWD -d $< -rst

$(call document_extension,st_flash,Write <...>.bin to a device using a ST-Link and STM32_Programmer_CLI.)

$(OUTPUT_DIRECTORY)/%.elf.size_tree: $(OUTPUT_DIRECTORY)/%.elf
	$(PYTHON) $(PATH_haussmann)/data/device/elf_size_tree.py $<

$(call document_extension,elf.size_tree,Generate a size tree from the ELF file.)
