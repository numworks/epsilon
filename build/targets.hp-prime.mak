products += epsilon.bin

%.bin: %.$(EXE)
	@echo "OBJCOPY $@"
	$(Q) $(OBJCOPY) --pad-to 0x30100000 -O binary $< $@
