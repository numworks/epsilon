products += $(patsubst %.$(EXE),%.hex,$(filter %.$(EXE),$(products)))
products += $(patsubst %.$(EXE),%.bin,$(filter %.$(EXE),$(products)))
products += $(patsubst %.$(EXE),%.map,$(filter %.$(EXE),$(products)))

%.hex: %.$(EXE)
	@echo "OBJCOPY $@"
	$(Q) $(OBJCOPY) -O ihex $< $@

%.bin: %.$(EXE)
	@echo "OBJCOPY $@"
	$(Q) $(OBJCOPY) -O binary $< $@

.PHONY: %_size
%_size: %.$(EXE)
	@echo "========= BUILD OUTPUT ========"
	@echo "File:  $<"
	@$(SIZE) $< | tail -n 1 | awk '{print "Code:  " $$1 " bytes";print "Data:  " $$2 " bytes"; print "Total: " int(($$1+$$2)/1024) " kB (" $$1 + $$2 " bytes)";}'
	@echo "==============================="

.PHONY: %_run
%_run: %.$(EXE)
	$(GDB) -x build/device/gdb_script.gdb $<

%.map: %.elf
	@echo "LDMAP   $@"
	$(Q) $(LD) $^ $(LDFLAGS) -M -Map $@ -o /dev/null

.PHONY: %_memory_map
%_memory_map: %.map
	@echo "========== MEMORY MAP ========="
	$(Q) awk -f build/device/memory_map.awk < $<
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
	openocd -f build/device/openocd.cfg

ifeq ($(EPSILON_USB_DFU_XIP)$(EPSILON_DEVICE_BENCH),10)
flasher.$(EXE): LDFLAGS = --gc-sections -T ion/src/device/usb/flasher.ld
flasher.$(EXE): $(objs) $(usb_objs) ion/src/device/usb/flasher.o
else
flasher.$(EXE):
	@echo "Error: flasher.elf requires EPSILON_DEVICE_BENCH=0 EPSILON_USB_DFU_XIP=1"
endif

products += flasher.$(EXE) flasher.bin
