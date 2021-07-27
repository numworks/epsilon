include build/targets.$(PLATFORM).userland.mak

HANDY_TARGETS += userland.A userland.B userland.test.A userland.test.B

userland_obj = $(call flavored_object_for,$(userland_src),leaveuserland consoledisplay official onboarding beta)
userland_test_obj = $(call flavored_object_for,$(userland_test_src),leaveuserland consoledisplay)

$(BUILD_DIR)/userland.A.$(EXE): $(userland_obj)
$(BUILD_DIR)/userland.B.$(EXE): $(userland_obj)
$(BUILD_DIR)/userland.test.A.$(EXE): $(userland_test_obj)
$(BUILD_DIR)/userland.test.B.$(EXE): $(userland_test_obj)
$(BUILD_DIR)/userland.%.$(EXE): LDFLAGS += $(USERLAND_LDFLAGS)
$(BUILD_DIR)/userland.test.%.$(EXE): USERLAND_LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/userland/$(subst .test.,_,$*)_flash.ld
$(BUILD_DIR)/userland.%.$(EXE): LDSCRIPT = $(USERLAND_LDSCRIPT)
$(BUILD_DIR)/userland.%.$(EXE): LDDEPS += $(USERLAND_LDDEPS)
