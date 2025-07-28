userland_src += $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_src) $(omg_src)

userland_test_src += $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_tests_src) $(omg_src) $(tests_src) $(runner_src)

# Ensure kandinsky fonts are generated first
$(call object_for,$(userland_src)): $(kandinsky_deps)

USERLAND_LDFLAGS += -Lion/src/$(PLATFORM)/userland/flash
USERLAND_LDDEPS += ion/src/$(PLATFORM)/userland/flash/userland_shared.ld

userland_target_variants = $(call add_slot_suffix, $(call target_variants_for_component,userland))
userland_targets = $(addprefix $(BUILD_DIR)/,$(addsuffix .$(EXE),$(userland_target_variants)))

HANDY_TARGETS += $(userland_target_variants)

$(foreach target,$(userland_targets),$(eval $(call flavored_dependencies_for_target,$(target),userland,consoledisplay leaveuserland)))

$(userland_targets): LDFLAGS += $(USERLAND_LDFLAGS)

$(filter %.A.$(EXE),$(userland_targets)): LDSCRIPT = ion/src/$(PLATFORM)/userland/flash/userland_A.ld
$(filter %.B.$(EXE),$(userland_targets)): LDSCRIPT = ion/src/$(PLATFORM)/userland/flash/userland_B.ld

$(userland_targets): LDDEPS += $(USERLAND_LDDEPS) $(LDSCRIPT)
