include build/targets.device.$(MODEL).mak
-include build/targets.device.$(FIRMWARE_COMPONENT).mak

HANDY_TARGETS += flasher.light flasher.verbose flasher.verbose.flash bench.ram bench.flash bootloader kernel.A kernel.B userland.A userland.B
HANDY_TARGETS_EXTENSIONS += dfu hex bin

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

$(BUILD_DIR)/%.$(EXE): LDDEPS += $(LDSCRIPT) ion/src/$(PLATFORM)/$(MODEL)/shared/config_flash.ld ion/src/$(PLATFORM)/shared/shared_config_flash.ld

$(BUILD_DIR)/%.A.$(EXE): LDDEPS += ion/src/$(PLATFORM)/$(MODEL)/shared/config_slot_a.ld
$(BUILD_DIR)/%.B.$(EXE): LDDEPS += ion/src/$(PLATFORM)/$(MODEL)/shared/config_slot_b.ld

# TODO: clean the BUILD_DIR definition in targets.device.mak to avoid requiring FIRMWARE_COMPONENT to:
# - find the right subfolder 'bootloader', 'kernel' or 'userland'
# - be able to rely on the usual rule for dfu
# - avoid requiring a standalone Makefile
#epsilon.dfu: DFUFLAGS += --signer $(BUILD_DIR)/signer --custom
#$(BUILD_DIR)/epsilon.dfu: $(BUILD_DIR)/userland.A.elf $(BUILD_DIR)/kernel.A.elf $(BUILD_DIR)/userland.B.elf $(BUILD_DIR)/kernel.B.elf $(BUILD_DIR)/signer

epsilon.dfu: DFUFLAGS += --signer $(BUILD_DIR)/signer --custom

.PHONY: $(BUILD_DIR)/epsilon.dfu
$(BUILD_DIR)/epsilon.dfu: $(BUILD_DIR)/signer
	$(MAKE) FIRMWARE_COMPONENT=bootloader bootloader.elf
	$(MAKE) FIRMWARE_COMPONENT=kernel kernel.A.elf
	$(MAKE) FIRMWARE_COMPONENT=kernel kernel.B.elf
	$(MAKE) FIRMWARE_COMPONENT=userland userland.A.elf
	$(MAKE) FIRMWARE_COMPONENT=userland userland.B.elf
	$(PYTHON) build/device/elf2dfu.py $(DFUFLAGS) -i \
	  $(subst epsilon,bootloader,$(BUILD_DIR)/bootloader.elf) \
	  $(subst epsilon,userland,$(BUILD_DIR)/userland.A.elf) \
	  $(subst epsilon,kernel,$(BUILD_DIR)/kernel.A.elf) \
	  $(subst epsilon,userland,$(BUILD_DIR)/userland.B.elf) \
	  $(subst epsilon,kernel,$(BUILD_DIR)/kernel.B.elf) \
	  -o $@
