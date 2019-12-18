include build/targets.device.$(MODEL).mak

executables += flasher.light flasher.verbose bench.ram bench.flash
extensions += .dfu .hex .bin

$(eval $(call rule_for, \
  DFUSE, %.dfu, %.$$(EXE), \
  $$(PYTHON) build/device/elf2dfu.py $$< $$@, \
  with_local_version \
))

$(eval $(call rule_for, \
  OBJCOPY, %.hex, %.$$(EXE), \
  $$(OBJCOPY) -O ihex $$< $$@ \
))

$(eval $(call rule_for, \
  OBJCOPY, %.bin, %.$$(EXE), \
  $$(OBJCOPY) -O binary $$< $$@, \
  with_local_version \
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
$(BUILD_DIR)/flasher.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/flasher
$(BUILD_DIR)/flasher.%.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/shared/ram.ld
flasher_objs = $(call object_for,$(ion_src) $(liba_src) $(kandinsky_src) $(flasher_src) $(ion_device_dfu_xip_src))
$(BUILD_DIR)/flasher.light.$(EXE): $(BUILD_DIR)/ion/src/$(PLATFORM)/flasher/display_light.o $(flasher_objs)
$(BUILD_DIR)/flasher.verbose.$(EXE): $(BUILD_DIR)/ion/src/$(PLATFORM)/flasher/display_verbose.o $(flasher_objs)

#TODO Do not build all apps... Put elsewhere?
$(BUILD_DIR)/bench.ram.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/bench
$(BUILD_DIR)/bench.ram.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/shared/ram.ld
$(BUILD_DIR)/bench.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/internal_flash.ld
bench_objs = $(call object_for,$(ion_src) $(liba_src) $(kandinsky_src) $(ion_device_dfu_xip_src) $(poincare_src) $(libaxx_src) $(bench_src) $(app_shared_src))
$(BUILD_DIR)/bench.ram.$(EXE): $(bench_objs)
$(BUILD_DIR)/bench.flash.$(EXE): $(bench_objs)
