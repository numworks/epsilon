
%.3dsx: %.elf
	$(Q) echo "3DSX    $(notdir $@)"
	$(Q) 3dsxtool $< $@
