HANDY_TARGETS += flasher
READ_ONLY ?= 0

flasher_src = $(ion_device_flasher_src) $(liba_src) $(liba_flasher_src) $(kandinsky_src)
$(BUILD_DIR)/flasher.$(EXE): $(call flavored_object_for,$(flasher_src),$(MODEL))
$(BUILD_DIR)/flasher.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-core/device/flasher
$(BUILD_DIR)/flasher.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-core/device/shared-core/flash
$(BUILD_DIR)/flasher.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/flasher/ram.ld
$(BUILD_DIR)/flasher.$(EXE): LDDEPS += $(LDSCRIPT)
