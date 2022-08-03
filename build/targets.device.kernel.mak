kernel_src = $(ion_device_kernel_src) $(liba_kernel_src) $(kandinsky_minimal_src)

KERNEL_LDFLAGS = -Lion/src/$(PLATFORM)/epsilon-core/device/kernel/flash -Lion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/$(MODEL)
KERNEL_LDDEPS += ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_shared.ld ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/$(MODEL)/canary.ld ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/$(MODEL)/prologue.ld

HANDY_TARGETS += kernel.A kernel.B

# stack protector
SFLAGS += -fstack-protector-strong

kernel_obj = $(call flavored_object_for,$(kernel_src),$(MODEL) $(THIRD_PARTY_FLAVOR))
ifeq ($(EMBED_EXTRA_DATA),1)
kernel_obj += $(BUILD_DIR)/bootloader.o $(BUILD_DIR)/trampoline.o
endif

$(BUILD_DIR)/kernel.A.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.A.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_A.ld

$(BUILD_DIR)/kernel.B.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.B.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_B.ld

$(BUILD_DIR)/kernel.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-core/device/kernel/flash -Lion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/$(MODEL)
$(BUILD_DIR)/kernel.%.$(EXE): LDDEPS += $(KERNEL_LDDEPS) $(LDSCRIPT)
