include build/targets.$(PLATFORM).userland.mak

HANDY_TARGETS += userland

userland_src += $(liba_internal_flash_src)
userland_obj = $(call flavored_object_for,$(userland_src),leaveuserland consoledisplay official onboarding beta)
$(BUILD_DIR)/userland.$(EXE): $(userland_obj)
$(BUILD_DIR)/userland.$(EXE): LDFLAGS += $(USERLAND_LDFLAGS)
$(BUILD_DIR)/userland.$(EXE): LDSCRIPT = $(USERLAND_LDSCRIPT)
$(BUILD_DIR)/userland.$(EXE): LDDEPS += $(USERLAND_LDDEPS)
