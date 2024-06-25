$(OUTPUT_DIRECTORY)/safe_stack/%: SFLAGS += -fstack-protector-strong

$(call create_goal,bootloader, \
  ion.bootloader \
  kandinsky.minimal \
  liba.minimal \
  libaxx \
  libsodium \
  omg.minimal.decompress \
,safe_stack, \
)

$(call create_goal,kernel, \
  ion.kernel \
  kandinsky.minimal \
  liba.armv7m \
  libaxx \
  omg.minimal \
,safe_stack, \
)

$(call create_goal,userland, \
  apps \
  escher \
  ion.userland \
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
  ion.userland.test \
  kandinsky.test \
  liba.aeabirt.armv7m.openbsd.test \
  libaxx.test \
  omg.test \
  poincare.test \
  python.test \
  quiz \
)

$(call create_goal,flasher, \
  ion.flasher \
  kandinsky.minimal \
  liba.minimal \
  libaxx \
  omg.minimal \
)

ifeq ($(PLATFORM),n0120)
flasher%flash: DFULEAVE := 0x24030000
else
flasher%flash: DFULEAVE := 0x20030000
endif

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
	$(QUIET) $(PYTHON) $(PATH_haussmann)/data/device/elf2dfu.py -i $$^ -o $$@
)
endef

$(call rule_for_composite_dfu,epsilon,$(addprefix safe_stack/,bootloader.elf kernel.A.elf kernel.B.elf) userland.A%elf userland.B%elf)
$(call rule_for_composite_dfu,test,$(addprefix safe_stack/,bootloader.elf kernel.A.elf kernel.B.elf) userland_test.A%elf userland_test.B%elf)
