userland_target_variants = $(subst .epsilon,,$(addprefix userland.,$(epsilon_target_variants)))

userland_src += $(liba_internal_flash_src)
userland_test_src += $(liba_internal_flash_src)

include build/targets.$(PLATFORM).userland.mak

$(userland_targets): LDDEPS = $(USERLAND_LDDEPS) $(LDSCRIPT)
