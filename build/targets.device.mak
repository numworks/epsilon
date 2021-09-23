epsilon_target_variants = test epsilon $(addprefix epsilon.,$(epsilon_flavors))
dfu_targets = $(addprefix $(BUILD_DIR)/,$(addsuffix .dfu,$(epsilon_target_variants)))
.PHONY: $(dfu_targets)
$(dfu_targets): USERLAND_STEM = $(subst .epsilon,,$(patsubst $(BUILD_DIR)/%.dfu,.%,$@))

include build/targets.device.$(MODEL).mak
-include build/targets.device.$(MODEL).$(FIRMWARE_COMPONENT).mak

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

$(BUILD_DIR)/%.$(EXE): LDDEPS += ion/src/$(PLATFORM)/shared/flash/$(MODEL)/config_flash.ld ion/src/$(PLATFORM)/shared/flash/$(MODEL)/config_sram.ld
$(BUILD_DIR)/%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared/flash -Lion/src/$(PLATFORM)/shared/flash/$(MODEL)
