userland_target_simple_variants = $(subst .epsilon,,$(addprefix userland.,$(epsilon_target_variants)))
userland_target_variants = $(addsuffix .A,$(userland_target_simple_variants)) $(addsuffix .B,$(userland_target_simple_variants))

include build/targets.$(PLATFORM).userland.mak

$(filter %.A.$(EXE),$(userland_targets)): LDSCRIPT = ion/src/$(PLATFORM)/userland/flash/userland_A_flash.ld
$(filter %.B.$(EXE),$(userland_targets)): LDSCRIPT = ion/src/$(PLATFORM)/userland/flash/userland_B_flash.ld

$(userland_targets): LDDEPS += $(USERLAND_LDDEPS) $(LDSCRIPT)
