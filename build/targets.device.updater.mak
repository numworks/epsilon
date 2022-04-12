HANDY_TARGETS += updater

updater_src = $(ion_device_updater_src) $(liba_internal_flash_src) $(kandinsky_minimal_src)
updater_obj = $(call flavored_object_for,$(updater_src),$(MODEL))
$(BUILD_DIR)/updater.$(EXE): $(updater_obj)

$(BUILD_DIR)/updater.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-bootloader/updater/flash/
$(BUILD_DIR)/updater.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-bootloader/updater/flash/ram.ld
$(BUILD_DIR)/updater.$(EXE): LDDEPS += $(LDSCRIPT)

# Rules to build the .dfu combining updater (RAM) and payload (FLASH)
$(BUILD_DIR)/payload.elf: $(BUILD_DIR)/bootloader.o
$(BUILD_DIR)/payload.elf: LDSCRIPT = ion/src/device/epsilon-bootloader/updater/flash/payload.ld
$(BUILD_DIR)/payload.elf: LDDEPS += $(LDSCRIPT)

$(BUILD_DIR)/updater.dfu: $(BUILD_DIR)/updater.elf $(BUILD_DIR)/payload.elf
	$(PYTHON) build/device/elf2dfu.py -i $^ -o $@
