HANDY_TARGETS += userland.A userland.B

USBXIP := $(if $(filter-out n0100,$(MODEL)),usbxip,)
userland_src = $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_src)
userland_obj = $(call flavored_object_for,$(userland_src),$(USBXIP) leaveauthentify consoledisplay official.onboarding.beta)
$(BUILD_DIR)/userland.A.$(EXE): $(userland_obj)
$(BUILD_DIR)/userland.B.$(EXE): $(userland_obj)
$(BUILD_DIR)/userland.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/userland
$(BUILD_DIR)/userland.%.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/userland/$(subst .,_,$*)_flash.ld
$(BUILD_DIR)/userland.%.$(EXE): LDDEPS += $(LDSCRIPT) ion/src/$(PLATFORM)/userland/userland_flash.ld ion/src/$(PLATFORM)/$(MODEL)/shared/config_flash.ld ion/src/$(PLATFORM)/shared/shared_config_flash.ld
