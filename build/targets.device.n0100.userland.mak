include build/targets.$(PLATFORM).userland.mak

HANDY_TARGETS += userland userland.test

userland_src += $(liba_internal_flash_src)
userland_obj = $(call flavored_object_for,$(userland_src),leaveuserland consoledisplay official onboarding beta)
userland_test_src += $(liba_internal_flash_src)
userland_test_obj = $(call flavored_object_for,$(userland_test_src),leaveuserland consoledisplay)

$(BUILD_DIR)/userland.$(EXE): $(userland_obj)
$(BUILD_DIR)/userland.test.$(EXE): $(userland_test_obj)
$(BUILD_DIR)/userland%$(EXE): LDFLAGS += $(USERLAND_LDFLAGS)
$(BUILD_DIR)/userland.test.$(EXE): USERLAND_LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/userland/userland_flash.ld
$(BUILD_DIR)/userland%$(EXE): LDSCRIPT = $(USERLAND_LDSCRIPT)
# Without the 'private' modifier, the assignment of USERLAND_LDDEPS to LDDEPS
# would be inherited by .elf prerequisites of userland.elf. If USERLAND_LDDEPS
# expands to an undefined file for these targets, it would cause the .elf build
# rule to be ignored.
$(BUILD_DIR)/userland%$(EXE): private LDDEPS += $(USERLAND_LDDEPS)
