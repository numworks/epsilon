userland_src = $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_src)

USERLAND_LDFLAGS += -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/$(MODEL)/userland -Lion/src/$(PLATFORM)/userland
USERLAND_LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/userland/$(subst .,_,$*)_flash.ld
USERLAND_LDDEPS += $(USERLAND_LDSCRIPT) ion/src/$(PLATFORM)/userland/userland_flash.ld
