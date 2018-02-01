products += $(products:.$(EXE)=.hex) $(products:.$(EXE)=.bin) $(products:.$(EXE)=.map)

%.hex: %.$(EXE)
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O ihex $< $@

%.bin: %.$(EXE)
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@

.PHONY: %_size
%_size: %.$(EXE)
	@echo "========= BUILD OUTPUT ========"
	@echo "File:  $<"
	@$(SIZE) $< | tail -n 1 | awk '{print "Code:  " $$1 " bytes";print "Data:  " $$2 " bytes"; print "Total: " int(($$1+$$2)/1024) " kB (" $$1 + $$2 " bytes)";}'
	@echo "==============================="

.PHONY: %_run
%_run: %.$(EXE)
	$(GDB) -x gdb_script.gdb $<

%.map: %.elf
	@echo "LDMAP   $@"
	$(Q) $(LD) $^ $(LDFLAGS) -M -Map $@ -o /dev/null

.PHONY: %_memory_map
%_memory_map: %.map
	@echo "========== MEMORY MAP ========="
	$(Q) awk -f ion/src/device/boot/memory_map.awk < $<
	@echo "==============================="

.PHONY: %_flash
%_flash: %.bin
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press the RESET button the back of your device."
	@until dfu-util -l | grep "Internal Flash" > /dev/null 2>&1; do sleep 1;done
	@echo "DFU     $@"
	@dfu-util -i 0 -a 0 -s 0x08000000:leave -D $<
