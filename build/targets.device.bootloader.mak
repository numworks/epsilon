HANDY_TARGETS += bootloader

ASSERTIONS = $(DEVELOPMENT)

# stack protector
SFLAGS += -fstack-protector-strong

bootloader_src = $(ion_device_bootloader_src) $(liba_internal_flash_src) $(kandinsky_minimal_src) $(libaxx_src)

# Ensure kandinsky fonts are generated first
$(call object_for,$(bootloader_src)): $(kandinsky_deps)

bootloader_obj = $(call flavored_object_for,$(bootloader_src),leavebootloader $(MODEL))
$(BUILD_DIR)/bootloader.$(EXE): $(bootloader_obj)

$(BUILD_DIR)/bootloader.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-bootloader/bootloader/flash/$(MODEL)
$(BUILD_DIR)/bootloader.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-bootloader/bootloader/flash/bootloader_flash.ld
$(BUILD_DIR)/bootloader.$(EXE): LDDEPS += $(LDSCRIPT)
