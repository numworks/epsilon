%.hex: %.$(EXE)
	@echo "OBJCOPY $@"
	$(Q) $(OBJCOPY) -O ihex $< $@

%.bin: %.$(EXE)
	@echo "OBJCOPY $@"
	$(Q) $(OBJCOPY) -O binary $< $@
# We pad the device binary files because there was a bug in an older version of
# the dfu code, and it did not upload properly a binary of length non-multiple
# of 32 bits.
#TODO: We over-pad here, pad with the the needed amount of bytes only.
	@echo "Padding $@"
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $@

.PHONY: %_size
%_size: %.$(EXE)
	@echo "========= BUILD OUTPUT ========"
	@echo "File:  $<"
	@$(SIZE) $< | tail -n 1 | awk '{print "Code:  " $$1 " bytes";print "Data:  " $$2 " bytes"; print "Total: " int(($$1+$$2)/1024) " kB (" $$1 + $$2 " bytes)";}'
	@echo "==============================="

.PHONY: %_run
%_run: %.$(EXE)
	$(GDB) -x scripts/device/gdb_script.gdb $<

%.map: %.elf
	@echo "LDMAP   $@"
	$(Q) $(LD) $^ $(LDFLAGS) -Wl,-M -Wl,-Map=$@ -o /dev/null

.PHONY: %_memory_map
%_memory_map: %.map
	@echo "========== MEMORY MAP ========="
	$(Q) awk -f scripts/device/memory_map.awk < $<
	@echo "==============================="

.PHONY: %_flash
%_flash: ./build/device/%.bin
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press the RESET button the back of your device."
	@until dfu-util -l | grep "Flash" > /dev/null 2>&1; do sleep 1;done
	@echo "DFU     $@"
	$(Q) dfu-util -i 0 -a 0 -s 0x08000000:leave -D $<

.PHONY: openocd
openocd:
	openocd -f scripts/device/openocd.cfg

ifeq ($(EPSILON_USB_DFU_XIP)$(EPSILON_DEVICE_BENCH),10)
$(BUILD_DIR)/flasher.$(EXE): LDSCRIPT = ion/src/device/usb/flasher.ld
$(BUILD_DIR)/flasher.$(EXE): $(objs) $(usb_objs) $(BUILD_DIR)/ion/src/device/usb/flasher.o
else
$(BUILD_DIR)/flasher.$(EXE):
	@echo "Error: flasher.elf requires EPSILON_DEVICE_BENCH=0 EPSILON_USB_DFU_XIP=1"
endif
