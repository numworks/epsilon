%.dfu: %.$(EXE)
	@echo "DFUSE   $@"
	$(Q) $(PYTHON) scripts/device/elf2dfu.py $< $@

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
	$(GDB) -x scripts/$(PLATFORM)/gdb_script.gdb $<

%.map: %.elf
	@echo "LDMAP   $@"
	$(Q) $(LD) $^ $(LDFLAGS) -Wl,-M -Wl,-Map=$@ -o /dev/null

.PHONY: %_memory_map
%_memory_map: %.map
	@echo "========== MEMORY MAP ========="
	$(Q) awk -f scripts/device/memory_map.awk < $<
	@echo "==============================="

.PHONY: %_flash
%_flash: %.bin
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press the RESET button the back of your device."
	@until dfu-util -l | grep "Flash" > /dev/null 2>&1; do sleep 1;done
	@echo "DFU     $@"
	$(Q) dfu-util -i 0 -a 0 -s 0x08000000:leave -D $<

.PHONY: openocd
openocd:
	openocd -f scripts/$(PLATFORM)/openocd.$(MODEL).cfg

# The flasher target is defined here because otherwise $(objs) has not been
# fully filled
ifeq ($(EPSILON_USB_DFU_XIP)$(EPSILON_DEVICE_BENCH),10)
ifdef EPSILON_FLASHER_VERBOSE
$(call object_for,ion/src/$(PLATFORM)/shared/usb/flasher.cpp): SFLAGS += -DEPSILON_FLASHER_VERBOSE=$(EPSILON_FLASHER_VERBOSE)
$(BUILD_DIR)/flasher.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/flasher
$(BUILD_DIR)/flasher.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/shared/ram.ld
$(BUILD_DIR)/flasher.$(EXE): $(objs) $(BUILD_DIR)/ion/src/$(PLATFORM)/shared/usb/flasher.o
else
$(BUILD_DIR)/flasher.$(EXE):
	@echo "Error: flasher requires EPSILON_FLASHER_VERBOSE to be set to 0 or 1"
endif
else
$(BUILD_DIR)/flasher.$(EXE):
	@echo "Error: flasher.elf requires EPSILON_DEVICE_BENCH=0 EPSILON_USB_DFU_XIP=1"
endif

#TODO Do not build all apps... Put elsewhere?
ifeq ($(EPSILON_USB_DFU_XIP)$(EPSILON_DEVICE_BENCH),11)
$(BUILD_DIR)/benchRAM.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/bench
$(BUILD_DIR)/benchRAM.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/shared/ram.ld
$(BUILD_DIR)/benchRAM.$(EXE): $(objs) $(call object_for,$(bench_src))
else
$(BUILD_DIR)/benchRAM.$(EXE):
	@echo "Error: benchRAM.bin requires EPSILON_DEVICE_BENCH=1 EPSILON_USB_DFU_XIP=1"
endif

#TODO Do not build all apps... Put elsewhere?
ifeq ($(EPSILON_USB_DFU_XIP)$(EPSILON_DEVICE_BENCH),11)
$(BUILD_DIR)/benchFlash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/internal_flash.ld
$(BUILD_DIR)/benchFlash.$(EXE): $(objs) $(call object_for,$(bench_src))
else
$(BUILD_DIR)/benchFlash.$(EXE):
	@echo "Error: benchFlash.bin requires EPSILON_DEVICE_BENCH=1 EPSILON_USB_DFU_XIP=1"
endif

ifeq ($(EPSILON_USB_DFU_XIP)$(EPSILON_DEVICE_BENCH)$(EPSILON_ONBOARDING_APP)$(EPSILON_BOOT_PROMPT),001update)
.PHONY: %_two_binaries
%_two_binaries: %.elf
	@echo "Building an internal and an external binary for     $<"
	$(Q) $(OBJCOPY) -O binary -j .text.external -j .rodata.external $< $(basename $<).external.bin
	$(Q) $(OBJCOPY) -O binary -R .text.external -R .rodata.external $< $(basename $<).internal.bin
	@echo "Padding $(basename $<).external.bin and $(basename $<).internal.bin"
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(basename $<).external.bin
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(basename $<).internal.bin
else
%_two_binaries:
	@echo "Error: two_binaries requires EPSILON_DEVICE_BENCH=0 EPSILON_USB_DFU_XIP=0 EPSILON_ONBOARDING_APP=1 EPSILON_BOOT_PROMPT=update"
endif

include scripts/targets.device.$(MODEL).mak
