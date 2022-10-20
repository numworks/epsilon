kernel_target_simple_variants = $(subst .epsilon,,$(addprefix kernel.,$(epsilon_target_variants)))
kernel_target_variants = $(addsuffix .A,$(kernel_target_simple_variants)) $(addsuffix .B,$(kernel_target_simple_variants))

HANDY_TARGETS += $(kernel_target_variants)

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

kernel_targets = $(addprefix $(BUILD_DIR)/,$(addsuffix .$(EXE),$(kernel_target_variants)))

KERNEL_LDFLAGS = -Lion/src/$(PLATFORM)/epsilon-core/device/kernel/flash
KERNEL_LDDEPS += ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_shared.ld

# stack protector
SFLAGS += -fstack-protector-strong

define rule_for_flavored_kernel
$(warning $$(call flavored_object_for,$$(kernel_src), $(MODEL) $(KERNEL_ASSERT_FLAVOR) $(patsubst $(BUILD_DIR)/kernel%.$(EXE),%,$(1))))
$(1): $$(call flavored_object_for,$$(kernel_src), $(MODEL) $(KERNEL_ASSERT_FLAVOR) $(patsubst $(BUILD_DIR)/kernel%.$(EXE),%,$(1)))
ifeq ($(EMBED_EXTRA_DATA),1)
$(1): $(BUILD_DIR)/trampoline.o $(BUILD_DIR)/bootloader.o
endif
endef

$(foreach target,$(kernel_targets),$(eval $(call rule_for_flavored_kernel,$(target))))

$(kernel_targets): LDFLAGS += $(KERNEL_LDFLAGS)

$(filter %.A.$(EXE),$(kernel_targets)): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_A.ld
$(filter %.B.$(EXE),$(kernel_targets)): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/kernel/flash/kernel_B.ld

$(kernel_targets): LDDEPS += $(KERNEL_LDDEPS) $(LDSCRIPT)
