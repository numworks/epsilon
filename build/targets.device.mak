include build/targets.device.$(MODEL).mak

HANDY_TARGETS += flasher.light flasher.verbose flasher.verbose.flash bench.ram bench.flash
HANDY_TARGETS_EXTENSIONS += dfu hex bin

%_ram_map: %.$(EXE)
	$(PYTHON) build/device/ram_map.py $(BUILD_DIR)/$<
	
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
flasher_src = $(ion_src) $(ion_device_flasher_src) $(liba_src) $(simple_kandinsky_src)
$(BUILD_DIR)/flasher.light.$(EXE): $(call flavored_object_for,$(flasher_src),light usbxip)
$(BUILD_DIR)/flasher.verbose.$(EXE): $(call flavored_object_for,$(flasher_src),usbxip)
$(BUILD_DIR)/flasher.verbose.flash.$(EXE): $(call flavored_object_for,$(flasher_src))
$(BUILD_DIR)/flasher.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/flasher
$(BUILD_DIR)/flasher.%.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/shared/ram.ld
$(BUILD_DIR)/flasher.%.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/internal_flash.ld

#TODO Do not build all apps... Put elsewhere?
bench_src = $(ion_src) $(liba_src) $(default_kandinsky_src) $(poincare_src) $(libaxx_src) $(app_shared_src) $(ion_device_bench_src)
$(BUILD_DIR)/bench.ram.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart usbxip)
$(BUILD_DIR)/bench.ram.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/bench
$(BUILD_DIR)/bench.ram.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/shared/ram.ld
$(BUILD_DIR)/bench.flash.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart usbxip)
$(BUILD_DIR)/bench.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/internal_flash.ld

.PHONY: %.two_binaries
%.two_binaries: %.elf
	@echo "Building an internal and an external binary for     $<"
	$(Q) $(OBJCOPY) -O binary -j .text.external -j .rodata.external -j .exam_mode_buffer $< $(basename $<).external.bin
	$(Q) $(OBJCOPY) -O binary -R .text.external -R .rodata.external -R .exam_mode_buffer $< $(basename $<).internal.bin
	@echo "Padding $(basename $<).external.bin and $(basename $<).internal.bin"
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(basename $<).external.bin
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(basename $<).internal.bin

.PHONY: binpack
binpack: $(BUILD_DIR)/flasher.light.bin $(BUILD_DIR)/epsilon.onboarding.two_binaries
	rm -rf $(BUILD_DIR)/binpack
	mkdir -p $(BUILD_DIR)/binpack
	cp $(BUILD_DIR)/flasher.light.bin $(BUILD_DIR)/binpack
	cp $(BUILD_DIR)/epsilon.onboarding.internal.bin $(BUILD_DIR)/epsilon.onboarding.external.bin $(BUILD_DIR)/binpack
	cd $(BUILD_DIR) && for binary in flasher.light.bin epsilon.onboarding.internal.bin epsilon.onboarding.external.bin; do shasum -a 256 -b binpack/$${binary} > binpack/$${binary}.sha256;done
	cd $(BUILD_DIR) && tar cvfz binpack-$(MODEL)-`git rev-parse HEAD | head -c 7`.tgz binpack/*
	$(PYTHON) build/device/secure_ext.py $(BUILD_DIR)/epsilon.onboarding.external.bin
