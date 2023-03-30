HANDY_TARGETS += flasher
READ_ONLY ?= 0
SIGNABLE ?= 0

ifeq ($(READ_ONLY),0)
ifeq ($(SIGNABLE),1)
$(error A signable flasher cannot be allowed to write memory)
endif
endif

flasher_src = $(ion_device_flasher_src) $(liba_src) $(liba_flasher_src) $(kandinsky_src)
$(BUILD_DIR)/flasher.$(EXE): $(call flavored_object_for,$(flasher_src),$(MODEL))
$(BUILD_DIR)/flasher.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-core/device/flasher
$(BUILD_DIR)/flasher.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-core/device/shared-core/flash
ifeq ($(SIGNABLE),1)
$(BUILD_DIR)/flasher.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/flasher/signable_ram.ld
else
$(BUILD_DIR)/flasher.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/flasher/ram.ld
endif
$(BUILD_DIR)/flasher.$(EXE): LDDEPS += $(LDSCRIPT)
