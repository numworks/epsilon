include build/targets.$(PLATFORM).userland.mak

HANDY_TARGETS += userland.A userland.B

userland_obj = $(call flavored_object_for,$(userland_src),usbxip leaveuserland consoledisplay official onboarding beta)
$(BUILD_DIR)/userland.A.$(EXE): $(userland_obj)
$(BUILD_DIR)/userland.B.$(EXE): $(userland_obj)
$(BUILD_DIR)/userland.%.$(EXE): LDFLAGS += $(USERLAND_LDFLAGS)
$(BUILD_DIR)/userland.%.$(EXE): LDSCRIPT = $(USERLAND_LDSCRIPT)
$(BUILD_DIR)/userland.%.$(EXE): LDDEPS += $(USERLAND_LDDEPS)
