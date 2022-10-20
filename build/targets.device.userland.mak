userland_target_simple_variants = $(subst .epsilon,,$(addprefix userland.,$(epsilon_target_variants)))
userland_target_variants = $(addsuffix .A,$(userland_target_simple_variants)) $(addsuffix .B,$(userland_target_simple_variants))

HANDY_TARGETS += $(userland_target_variants)

userland_src += $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_src)

userland_test_src += $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_tests_src) $(tests_src) $(runner_src)

# Ensure kandinsky fonts are generated first
$(call object_for,$(userland_src)): $(kandinsky_deps)

userland_targets = $(addprefix $(BUILD_DIR)/,$(addsuffix .$(EXE),$(userland_target_variants)))

USERLAND_LDFLAGS += -Lion/src/$(PLATFORM)/userland/flash
USERLAND_LDDEPS += ion/src/$(PLATFORM)/userland/flash/userland_shared.ld

define rule_for_flavored_userland
$(1): $$(call flavored_object_for, \
	$(if $(findstring test,$(1)),$$(userland_test_src),$$(userland_src)), \
	leaveuserland consoledisplay $(MODEL) $(THIRD_PARTY_FLAVOR) $(patsubst $(BUILD_DIR)/userland%.$(EXE),%,$(1)))
endef

$(foreach target,$(userland_targets),$(eval $(call rule_for_flavored_userland,$(target))))

$(userland_targets): LDFLAGS += $(USERLAND_LDFLAGS)

$(filter %.A.$(EXE),$(userland_targets)): LDSCRIPT = ion/src/$(PLATFORM)/userland/flash/userland_A.ld
$(filter %.B.$(EXE),$(userland_targets)): LDSCRIPT = ion/src/$(PLATFORM)/userland/flash/userland_B.ld

$(userland_targets): LDDEPS += $(USERLAND_LDDEPS) $(LDSCRIPT)
