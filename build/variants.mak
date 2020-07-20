# Helper functions to work with variants
# Make variants_test for an example

define available_variants_in
$(sort $(patsubst +%,%,$(filter +%,$(subst :, ,$(1)))))
endef

define without_any_variant_specifier
$(filter-out $(foreach variant,$(call available_variants_in,$(1)),%:-$(variant) %:+$(variant)),$(1))
endef

define with_variant_specifier_matching
$(foreach variant,$(3),$(patsubst %:$(1)$(variant),%,$(filter %:$(1)$(variant),$(2))))
endef

define without_any_variant_specifier_matching
$(filter-out \
  $(call with_variant_specifier_matching,$(1),$(2),$(3)), \
  $(foreach variant,$(call available_variants_in,$(2)),$(call with_variant_specifier_matching,$(1),$(2),$(variant))) \
)
endef

# Return files in $(1) that match the variant $(2)
define filter_variants
$(sort \
  $(call without_any_variant_specifier,$(1)) \
  $(call with_variant_specifier_matching,+,$(1),$(2)) \
  $(call without_any_variant_specifier_matching,-,$(1),$(2)) \
)
endef

# Return all files in $(1) no matter their variant
define any_variant
$(sort $(filter-out -%,$(filter-out +%,$(subst :, ,$(1)))))
endef

# Examples

variants_test_src = base.cpp
variants_test_src += color/green.cpp:+green
variants_test_src += color/red.cpp:+red
variants_test_src += color/blue.cpp:-red
variants_test_src += color/blue.cpp:-green
variants_test_src += engine/fast.cpp:+nitro
variants_test_src += engine/slow.cpp:-nitro

.PHONY: variants_test
variants_test:
	$(info AVAILABLE_VARIANTS_IN)
	$(info --result:   $(call available_variants_in,$(variants_test_src),))
	$(info --expected: green nitro red)
	$(info FILTER_VARIANTS)
	$(info --result:   $(call filter_variants,$(variants_test_src),))
	$(info --expected: base.cpp color/blue.cpp engine/slow.cpp)
	$(info FILTER_VARIANTS red)
	$(info --result:   $(call filter_variants,$(variants_test_src),red))
	$(info --expected: base.cpp color/red.cpp engine/slow.cpp)
	$(info FILTER_VARIANTS green speed)
	$(info --result:   $(call filter_variants,$(variants_test_src),green nitro))
	$(info --expected: base.cpp color/green.cpp engine/fast.cpp)
	$(info ANY_VARIANT)
	$(info --result:   $(call any_variant,$(variants_test_src)))
	$(info --expected: base.cpp color/blue.cpp color/green.cpp color/red.cpp engine/fast.cpp engine/slow.cpp)
