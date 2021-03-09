include build/targets.device.$(MODEL).mak

HANDY_TARGETS += flasher.light flasher.verbose flasher.verbose.flash bench.ram bench.flash bootloader kernel userland
# TODO EMILIE: move rescue to N110 only
HANDY_TARGETS_EXTENSIONS += dfu hex bin

kernel.dfu: DFUFLAGS += --signed_with $(BUILD_DIR)/signer
userland.dfu: DFUFLAGS += --signed_with $(BUILD_DIR)/signer --sized

$(eval $(call rule_for, \
  RAMSIZE, %_ram_map.png, %.elf, \
  $$(PYTHON) build/device/ram_map.py $$< $$@, \
  local \
))

.PHONY: %_size
%_size: $(BUILD_DIR)/%.$(EXE)
	@echo "========= BUILD OUTPUT ========"
	@echo "File:  $<"
	@$(SIZE) $< | tail -n 1 | awk '{print "Code:  " $$1 " bytes";print "Data:  " $$2 " bytes"; print "Total: " int(($$1+$$2)/1024) " kB (" $$1 + $$2 " bytes)";}'
	@echo "==============================="

.PHONY: %_run
%_run: $(BUILD_DIR)/%.$(EXE)
	$(GDB) -x build/$(PLATFORM)/gdb_script.gdb $<

$(BUILD_DIR)/%.map: $(BUILD_DIR)/%.elf
	@echo "LDMAP   $@"
	$(Q) $(LD) $^ $(LDFLAGS) -Wl,-M -Wl,-Map=$@ -o /dev/null

.PHONY: %_memory_map
%_memory_map: $(BUILD_DIR)/%.map
	@echo "========== MEMORY MAP ========="
	$(Q) awk -f build/device/memory_map.awk < $<
	@echo "==============================="

.PHONY: openocd
openocd:
	openocd -f build/$(PLATFORM)/openocd.$(MODEL).cfg

# The flasher target is defined here because otherwise $(%_src) has not been
# fully filled
flasher_src = $(ion_src) $(ion_device_flasher_src) $(liba_src) $(kandinsky_src)
$(BUILD_DIR)/flasher.light.$(EXE): $(call flavored_object_for,$(flasher_src),light usbxip)
$(BUILD_DIR)/flasher.verbose.$(EXE): $(call flavored_object_for,$(flasher_src),usbxip)
$(BUILD_DIR)/flasher.verbose.flash.$(EXE): $(call flavored_object_for,$(flasher_src))
$(BUILD_DIR)/flasher.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/flasher
$(BUILD_DIR)/flasher.%.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/shared/ram.ld
$(BUILD_DIR)/flasher.%.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/internal_flash.ld

#TODO Do not build all apps... Put elsewhere?
bench_src = $(ion_src) $(liba_src) $(kandinsky_src) $(poincare_src) $(libaxx_src) $(app_shared_src) $(ion_device_bench_src)
$(BUILD_DIR)/bench.ram.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart usbxip)
$(BUILD_DIR)/bench.ram.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/bench
$(BUILD_DIR)/bench.ram.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/shared/ram.ld
$(BUILD_DIR)/bench.flash.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart usbxip)
$(BUILD_DIR)/bench.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/internal_flash.ld

bootloader_src = $(ion_device_bootloader_src) $(liba_bootloader_src)
$(BUILD_DIR)/bootloader.$(EXE): $(call flavored_object_for,$(bootloader_src),usbxip)
$(BUILD_DIR)/bootloader.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/bootloader
$(BUILD_DIR)/bootloader.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/bootloader/bootloader_flash.ld

kernel_src = $(ion_device_kernel_src) $(liba_kernel_src) $(kandinsky_kernel_src)
$(BUILD_DIR)/kernel.$(EXE): $(call flavored_object_for,$(kernel_src),)
$(BUILD_DIR)/kernel.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/$(MODEL)/kernel
$(BUILD_DIR)/kernel.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/kernel/kernel_flash.ld

userland_src = $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_src)
$(BUILD_DIR)/userland.$(EXE): $(call flavored_object_for,$(userland_src),consoledisplay onboarding)
$(BUILD_DIR)/userland.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/$(MODEL)/userland
$(BUILD_DIR)/userland.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/userland/userland_flash.ld
