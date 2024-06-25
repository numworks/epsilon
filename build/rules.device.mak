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

# Special target for the combined epsilon DFU
epsilon%dfu: $(OUTPUT_DIRECTORY)/epsilon%dfu
	@ :
epsilon%flash: $(OUTPUT_DIRECTORY)/epsilon%flash
	@ :

$(OUTPUT_DIRECTORY)/epsilon%dfu: $(addprefix $(OUTPUT_DIRECTORY)/,$(addprefix safe_stack/,bootloader.elf kernel.A.elf kernel.B.elf) userland.A%elf userland.B%elf)
	$(call rule_label,DFU)
	$(QUIET) $(PYTHON) $(PATH_haussmann)/data/device/elf2dfu.py -i $^ -o $@
