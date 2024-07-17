# Firmware component - bootloader

$(call create_goal,bootloader, \
  ion.bootloader \
  kandinsky.minimal \
  liba.minimal \
  libaxx \
  libsodium \
  omg.minimal.decompress \
,bootloader, \
)

$(OUTPUT_DIRECTORY)/bootloader/%.elf: SFLAGS += -fstack-protector-strong

# Firmware component - kernel

ifeq ($(ASSERTIONS),0)
$(call create_goal,kernel, \
  ion.kernel \
  kandinsky.minimal \
  liba.armv7m \
  libaxx \
  omg.minimal.decompress \
,kernel, \
)
else
$(call create_goal,kernel, \
  ion.kernel.kernelassert \
  kandinsky \
  liba.armv7m \
  libaxx \
  omg.minimal.decompress.utf8 \
,kernel, \
)
endif

$(OUTPUT_DIRECTORY)/kernel/%.elf: SFLAGS += -fstack-protector-strong

ifneq ($(DEBUG),0)
# Kernel without optimization is too large to fit in its 64k section.
$(OUTPUT_DIRECTORY)/kernel/%.elf: SFLAGS += -Os

HELP_GOAL_kernel := In debug mode the kernel is built with -Os to fit in its section
endif

# Firmware component - userland

$(call create_goal,userland, \
  apps \
  escher \
  ion.userland$(if $(filter 0,$(ASSERTIONS)),,.consoledisplay) \
  kandinsky \
  liba.aeabirt.armv7m.openbsd \
  libaxx \
  omg \
  poincare \
  python \
)

$(call create_goal,userland_test, \
  apps.test \
  escher.test \
  ion.userland.consoledisplay.test \
  kandinsky.test \
  liba.aeabirt.armv7m.openbsd.test \
  libaxx.test \
  omg.test \
  poincare.test \
  python.test \
  quiz \
)

# Firmware component - flasher

$(call create_goal,flasher, \
  ion.flasher \
  kandinsky.minimal \
  liba.minimal \
  libaxx \
  omg.minimal \
,,Building flasher.flash will automatically jump at the right address \
)

ifeq ($(PLATFORM),n0120)
flasher%flash: DFULEAVE := 0x24030000
else
flasher%flash: DFULEAVE := 0x20030000
endif

# Firmware component - bench

# TODO


# Rules for the composite DFUs made of several ELFs (e.g. epsilon.onboarding.dfu ...)

# rule_for_composite_dfu, <name>, <prerequisites, with optional % for the flavors>
define rule_for_composite_dfu
$(eval \
$1%dfu: $(OUTPUT_DIRECTORY)/$1%dfu
	@ :
$1%flash: $(OUTPUT_DIRECTORY)/$1%flash
	@ :

$(OUTPUT_DIRECTORY)/$1%dfu: $(addprefix $(OUTPUT_DIRECTORY)/,$2) | $$$$(@D)/.
	$$(call rule_label,DFU)
	$(PYTHON) $(PATH_haussmann)/data/device/elf2dfu.py -i $$^ -o $$@
)
endef

$(call rule_for_composite_dfu,epsilon,bootloader/bootloader.elf kernel/kernel.A%elf kernel/kernel.B%elf userland.A%elf userland.B%elf)
$(call document_other_target,epsilon.<flavors>.dfu,Composite DFU file made of a bootloader and two kernel&userland)

$(call rule_for_composite_dfu,test,bootloader/bootloader.elf kernel/kernel.A%elf kernel/kernel.B%elf userland_test.A%elf userland_test.B%elf)
$(call document_other_target,test.<flavors>.dfu)
