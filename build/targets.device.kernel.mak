HANDY_TARGETS += kernel.A kernel.B

# stack protector
SFLAGS += -fstack-protector-strong

kernel_src = $(ion_device_kernel_src) $(liba_kernel_src) $(kandinsky_minimal_src)
kernel_obj = $(call flavored_object_for,$(kernel_src),)
$(BUILD_DIR)/kernel.A.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.B.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/kernel
$(BUILD_DIR)/kernel.%.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/kernel/$(subst .,_,$*)_flash.ld
$(BUILD_DIR)/kernel.%.$(EXE): LDDEPS += ion/src/$(PLATFORM)/kernel/kernel_flash.ld
