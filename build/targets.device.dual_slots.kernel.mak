include build/targets.$(PLATFORM).kernel.mak

HANDY_TARGETS += kernel.A kernel.B

# stack protector
SFLAGS += -fstack-protector-strong

kernel_obj = $(call flavored_object_for,$(kernel_src),$(MODEL) $(THIRD_PARTY_FLAVOR))
kernel_obj += $(BUILD_DIR)/bootloader.o $(BUILD_DIR)/trampoline.o

$(BUILD_DIR)/kernel.A.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.B.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.%.$(EXE): LDFLAGS += $(KERNEL_LDFLAGS)
$(BUILD_DIR)/kernel.%.$(EXE): LDSCRIPT = $(KERNEL_LDSCRIPT)
$(BUILD_DIR)/kernel.%.$(EXE): LDDEPS += $(KERNEL_LDDEPS)
