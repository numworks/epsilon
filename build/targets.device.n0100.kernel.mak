include build/targets.$(PLATFORM).kernel.mak

HANDY_TARGETS += kernel

$(BUILD_DIR)/kernel.$(EXE): $(kernel_obj)
$(BUILD_DIR)/kernel.$(EXE): LDFLAGS += $(KERNEL_LDFLAGS)
$(BUILD_DIR)/kernel.$(EXE): LDSCRIPT = $(KERNEL_LDSCRIPT)
