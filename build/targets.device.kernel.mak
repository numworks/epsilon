kernel_src = $(ion_device_kernel_src) $(liba_kernel_src)

ASSERTIONS = $(DEVELOPMENT)
ifeq ($(ASSERTIONS),1)
KERNEL_ASSERT_FLAVOR = kernelassert
kernel_src += $(kandinsky_src)
else
kernel_src += $(kandinsky_minimal_src)
endif

# Ensure kandinsky fonts are generated first
$(call object_for,$(kernel_src)): $(kandinsky_deps)

KERNEL_LDFLAGS = -Lion/src/$(PLATFORM)/epsilon-core/device/kernel/flash
KERNEL_LDDEPS += ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_shared.ld

HANDY_TARGETS += kernel.A kernel.B

# stack protector
SFLAGS += -fstack-protector-strong

kernel_obj = $(call flavored_object_for,$(kernel_src),$(MODEL) $(THIRD_PARTY_FLAVOR) $(KERNEL_ASSERT_FLAVOR))
ifeq ($(EMBED_EXTRA_DATA),1)
kernel_obj += $(BUILD_DIR)/bootloader.o $(BUILD_DIR)/trampoline.o
endif

$(BUILD_DIR)/kernel.A.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.A.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_A.ld

$(BUILD_DIR)/kernel.B.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.B.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_B.ld

$(BUILD_DIR)/kernel.%.$(EXE): LDFLAGS += $(KERNEL_LDFLAGS)
$(BUILD_DIR)/kernel.%.$(EXE): LDDEPS += $(KERNEL_LDDEPS) $(LDSCRIPT)
