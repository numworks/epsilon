_ion_firmware_components := bench bootloader flasher kernel userland

-include $(patsubst %,$(PATH_ion)/shared.device.%.mak,$(_ion_firmware_components))

# TODO Add USB sources, but be smart about it: they use the -I shadowing trick
# to get a different config for each firmware component, and that won't work
# anymore.

SOURCES_ion += $(addprefix $(PATH_ion)/src/, \
  $(foreach c,$(_ion_firmware_components),$(addsuffix :+$c,$(_sources_ion_$c))) \
)

LDFLAGS_ion += \
  $(foreach c,$(_ion_firmware_components),$(addsuffix :+$c,$(_ldflags_ion_$c))) \
  -L$(PATH_ion)/src/device/shared/flash \
  -L$(OUTPUT_DIRECTORY)/ion/src/device/shared/flash

# TODO Add a mechanism to track LDDEPS
$(OUTPUT_DIRECTORY)/userland.A.elf: $(OUTPUT_DIRECTORY)/$(PATH_ion)/src/device/shared/flash/board.ld

$(OUTPUT_DIRECTORY)/$(PATH_ion)/src/device/shared/flash/board.ld: $(PATH_ion)/src/device/include/$(PLATFORM)/config/board.h | $$(@D)/.
	$(call rule_label,AWK)
	$(QUIET) $(CXX) $(SFLAGS) -E $< -o $(@:.ld=.h)
	$(QUIET) awk '/^constexpr/ {$$1=$$2=""; sub(";.*", ";"); print}; /^static_assert/ {sub("static_assert", "ASSERT"); print}' $(@:.ld=.h) >$@

# TODO Of those flags, phase out those that can be edited on the command line,
# namely DEVELOPMENT and IN_FACTORY
PRIVATE_SFLAGS_ion += \
  -DDEVELOPMENT=$(DEVELOPMENT) \
  -DIN_FACTORY=$(IN_FACTORY) \
  -DPCB_LATEST=$(PCB_LATEST) \
  -DSIGNATURE_INDEX=$(SIGNATURE_INDEX)

PRIVATE_SFLAGS_ion += \
  -I$(PATH_ion)/src/device/include/$(PLATFORM) \
  -I$(PATH_ion)/src/device
