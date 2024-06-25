# Helper functions to work with variants
# Make variants_test for an example

comma:= ,

define available_variants_in
$(sort $(patsubst +%,%,$(filter +%,$(subst $(comma), ,$(subst :, ,$(1))))))
endef

define with_variant_specifier
$(if $(3), \
$(foreach w,$(2),$(if $(findstring $(1)$(3),$(w)),$(firstword $(subst :, ,$(w))))), \
)
endef

# Return files in $(1) that match the variant $(2)
# # Consider all $(1) and then:
# - filter out the one with +variant_specifier that are not in $(2)
# - filter out the one with -variant_specifier that are in $(2)
define filter_variants
$(sort \
  $(filter-out \
    $(foreach variant,$(2),$(call with_variant_specifier,-,$(1),$(variant))), \
    $(filter-out \
      $(foreach variant,$(filter-out $(2),$(call available_variants_in,$(1))),$(call with_variant_specifier,+,$(1),$(variant))), \
      $(foreach w,$(1),$(firstword $(subst :, ,$(w)))) \
    ) \
  ) \
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
variants_test_src += combination/red_fast.cpp:+red,+nitro
variants_test_src += combination/slow_big.cpp:-nitro,+big

.PHONY: variants_test
variants_test:
	$(info AVAILABLE_VARIANTS_IN)
	$(info --result:   $(call available_variants_in,$(variants_test_src),))
	$(info --expected: big green nitro red)
	$(info FILTER_VARIANTS)
	$(info --result:   $(call filter_variants,$(variants_test_src),))
	$(info --expected: base.cpp color/blue.cpp engine/slow.cpp)
	$(info FILTER_VARIANTS red)
	$(info --result:   $(call filter_variants,$(variants_test_src),red))
	$(info --expected: base.cpp color/red.cpp engine/slow.cpp)
	$(info FILTER_VARIANTS green speed)
	$(info --result:   $(call filter_variants,$(variants_test_src),green nitro))
	$(info --expected: base.cpp color/green.cpp engine/fast.cpp)
	$(info FILTER_VARIANTS red nitro)
	$(info --result:   $(call filter_variants,$(variants_test_src),red nitro))
	$(info --expected: base.cpp color/red.cpp combination/red_fast.cpp engine/fast.cpp)
	$(info FILTER_VARIANTS big nitro)
	$(info --result:   $(call filter_variants,$(variants_test_src),big nitro))
	$(info --expected: base.cpp color/blue.cpp engine/fast.cpp)
	$(info FILTER_VARIANTS big)
	$(info --result:   $(call filter_variants,$(variants_test_src),big))
	$(info --expected: base.cpp color/blue.cpp combination/slow_big.cpp engine/slow.cpp)
	$(info ANY_VARIANT)
	$(info --result:   $(call any_variant,$(variants_test_src)))
	$(info --expected: base.cpp color/blue.cpp color/green.cpp color/red.cpp combination/red_fast.cpp combination/slow_big.cpp engine/fast.cpp engine/slow.cpp)
