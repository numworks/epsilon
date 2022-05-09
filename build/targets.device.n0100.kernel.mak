include build/targets.$(PLATFORM).kernel.mak

HANDY_TARGETS += kernel

kernel_src += $(liba_internal_flash_src)
kernel_obj = $(call flavored_object_for,$(kernel_src),n0100)
$(BUILD_DIR)/kernel.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.$(EXE): LDFLAGS += $(KERNEL_LDFLAGS)
$(BUILD_DIR)/kernel.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/kernel/flash/kernel.ld
$(BUILD_DIR)/kernel.$(EXE): LDDEPS += $(KERNEL_LDDEPS)
