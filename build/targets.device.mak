epsilon_target_variants = test epsilon $(addprefix epsilon.,$(epsilon_flavors))
dfu_targets = $(addprefix $(BUILD_DIR)/,$(addsuffix .dfu,$(epsilon_target_variants)))
.PHONY: $(dfu_targets)
$(dfu_targets): USERLAND_STEM = $(subst .noboot,,$(subst .epsilon,,$(patsubst $(BUILD_DIR)/%.dfu,.%,$@)))

ifeq ($(ALLOW_THIRD_PARTY),1)
THIRD_PARTY_FLAVOR=allow3rdparty
endif

include build/targets.device.$(MODEL).mak
-include build/targets.device.$(MODEL).$(FIRMWARE_COMPONENT).mak

HANDY_TARGETS_EXTENSIONS += dfu hex bin

$(eval $(call rule_for, \
  RAMSIZE, %_ram_map.png, %.elf, \
  $$(PYTHON) build/device/ram_map.py $$< $$@, \
  local \
))

$(BUILD_DIR)/immv.html: build/device/immv/$(MODEL).json $(BUILD_DIR)/kernel.A.sections.json $(BUILD_DIR)/kernel.A.symbols.json $(BUILD_DIR)/userland.A.sections.json $(BUILD_DIR)/userland.A.symbols.json
	@echo "INLINE  $@"
	$(Q) ion/src/simulator/web/inline.py --script build/device/immv/immv.js build/device/immv/immv.html > $@
	@echo "INJMAP  $@"
	$(Q) echo "<script>" >> $@
	$(Q) for MAP in $^;do echo "addMaps(" >> $@; cat $${MAP} >> $@;echo ")" >> $@;done
	$(Q) echo "</script>" >> $@

.PHONY: %_size
%_size: $(BUILD_DIR)/%.$(EXE)
	@echo "========= BUILD OUTPUT ========"
	@echo "File:  $<"
	@$(SIZE) $< | tail -n 1 | awk '{print "Code:  " $$1 " bytes";print "Data:  " $$2 " bytes"; print "Total: " int(($$1+$$2)/1024) " kB (" $$1 + $$2 " bytes)";}'
	@echo "==============================="

.PHONY: %_run
%_run: $(BUILD_DIR)/%.$(EXE)
	$(GDB) -x build/$(PLATFORM)/gdb_script.gdb $<

$(BUILD_DIR)/%.sections.json: $(BUILD_DIR)/%.elf
	@echo "ELF2JS  $@"
	$(Q) $(PYTHON) build/device/immv/elf2json.py -S $^ > $@

$(BUILD_DIR)/%.symbols.json: $(BUILD_DIR)/%.elf
	@echo "ELF2JS  $@"
	$(Q) $(PYTHON) build/device/immv/elf2json.py -s 20 $^ > $@

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

$(BUILD_DIR)/%.$(EXE): LDDEPS += ion/src/$(PLATFORM)/shared/flash/$(MODEL)/config_flash.ld ion/src/$(PLATFORM)/shared/flash/$(MODEL)/config_sram.ld
$(BUILD_DIR)/%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared/flash -Lion/src/$(PLATFORM)/shared/flash/$(MODEL)

$(BUILD_DIR)/bootloader.o:
	$(MAKE) FIRMWARE_COMPONENT=bootloader bootloader.bin
	$(Q) $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.bootloader $(subst $(FIRMWARE_COMPONENT),bootloader,$(BUILD_DIR))/bootloader.bin $@

