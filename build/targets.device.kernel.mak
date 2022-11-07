kernel_src = $(ion_device_kernel_src) $(liba_kernel_src)
kernel_test_src = $(kernel_src)

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

# stack protector
SFLAGS += -fstack-protector-strong

kernel_target_variants = $(call add_slot_suffix, $(call target_variants_for_component,kernel))
kernel_targets = $(addprefix $(BUILD_DIR)/,$(addsuffix .$(EXE),$(kernel_target_variants)))

HANDY_TARGETS += $(kernel_target_variants)

define embed_extra_data_dependencies
ifeq ($(EMBED_EXTRA_DATA),1)
$(1): $(BUILD_DIR)/trampoline.o $(BUILD_DIR)/bootloader.o
endif
endef

$(foreach target,$(kernel_targets),$(eval $(call flavored_dependencies_for_target,$(target),kernel,$(KERNEL_ASSERT_FLAVOR))))
$(foreach target,$(kernel_targets),$(eval $(call embed_extra_data_dependencies,$(target))))

$(kernel_targets): LDFLAGS += $(KERNEL_LDFLAGS)

$(filter %.A.$(EXE),$(kernel_targets)): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_A.ld
$(filter %.B.$(EXE),$(kernel_targets)): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_B.ld

$(kernel_targets): LDDEPS += $(KERNEL_LDDEPS) $(LDSCRIPT)