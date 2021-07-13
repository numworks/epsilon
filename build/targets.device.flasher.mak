HANDY_TARGETS += flasher

bootloader_src = $(ion_device_bootloader_src) $(liba_bootloader_src) $(kandinsky_minimal_src)
bootloader_obj = $(call flavored_object_for,$(bootloader_src),usbxip leavebootloader)
$(BUILD_DIR)/bootloader.$(EXE): $(bootloader_obj)

# The flasher target is defined here because otherwise $(%_src) has not been
# fully filled
flasher_src = $(ion_device_flasher_src) $(liba_src) $(liba_flasher_src) $(kandinsky_src)
$(BUILD_DIR)/flasher.$(EXE): $(call flavored_object_for,$(flasher_src), usbxip)
$(BUILD_DIR)/flasher.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/$(MODEL)/flasher
$(BUILD_DIR)/flasher.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/flasher/ram.ld
