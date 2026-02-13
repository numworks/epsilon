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

ifeq ($(DEBUG),1)
ifneq ($(PLATFORM),n0120)
# Bootloader without optimization is larger than the 64k of the STM32F7 internal
# flash
$(OUTPUT_DIRECTORY)/bootloader/%.elf: SFLAGS += -Os

HELP_GOAL_bootloader := In debug mode the bootloader is built with -Os to fit in STM32F7 internal flash
endif
endif

$(OUTPUT_DIRECTORY)/bootloader/%.program: $(OUTPUT_DIRECTORY)/bootloader/%.elf
	$(call rule_label,PROGRAM)
	openocd -f build/device/openocd.$(PLATFORM).cfg -c "unlock; program $<; reset; shutdown"

$(call document_other_target,bootloader.program,Unlock device and flash bootloader with openocd)

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

$(call create_goal,bench, \
  ion.bench \
  kandinsky \
  liba.armv7m.openbsd \
  libaxx \
  omg \
  poincare.nopool.defaultpreferences \
,, \
)

$(OUTPUT_DIRECTORY)/kernel/%.elf: SFLAGS += -fstack-protector-strong

ifeq ($(DEBUG),1)
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
  quiz.epsilon \
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

$(call create_goal,eraser, \
  ion.eraser \
  kandinsky.minimal \
  liba.minimal \
  libaxx \
  omg.minimal \
)

.PHONY: binpack
binpack:
ifndef IN_FACTORY
	@echo "\033[0;31mWARNING\033[0m You are building a binpack."
	@echo "You must specify where this binpack will be used."
	@echo "Please set the IN_FACTORY environment variable to either:"
	@echo "  - 0 for use in diagnostic"
	@echo "  - 1 for use in production"
	@exit -1
endif
ifeq ($(IN_FACTORY),0)
	@echo "\033[0;31mWARNING\033[0m You are building a binpack with IN_FACTORY=0"
endif
	rm -rf output/binpack
	mkdir -p output/binpack
	# $(MAKE) clean
	$(MAKE) IN_FACTORY=$(IN_FACTORY) flasher.bin bench.ram.dfu epsilon.onboarding.update.dfu
	cp $(subst binpack,flasher,$(OUTPUT_DIRECTORY))/flasher.bin output/binpack
	cp $(subst binpack,bench,$(OUTPUT_DIRECTORY)/bench.ram.dfu) output/binpack
	cp $(subst binpack,epsilon,$(OUTPUT_DIRECTORY))/epsilon.onboarding.update.dfu output/binpack
	# $(MAKE) clean
	cd output/binpack && for binary in flasher.bin; do shasum -a 256 -b $${binary} > $${binary}.sha256;done
	cd output && tar cvfz binpack-`git rev-parse HEAD | head -c 7`.$(PLATFORM).tgz binpack
	@echo "Binpack created as \033[0;32moutput/binpack-`git rev-parse HEAD | head -c 7`.$(PLATFORM).tgz\033[0m"
	@echo "\033[0;31mWARNING\033[0m Files bench.ram.dfu and epsilon.official.onboarding.update.dfu need to be signed"
	rm -rf output/binpack


eraser%: EMBED_EXTRA_DATA := 1

.PHONY: custom_userland.dfu
custom_userland.dfu:
	$(MAKE) userland.A.dfu userland.B.dfu

$(call document_other_target,custom_userland.dfu,Build firmware as a custom userland for any slot. APP_VERSION will have to match the official version of the device's official software.)

ifeq ($(PLATFORM),n0120)
flasher%flash: DFULEAVE := 0x24030000
eraser%flash: DFULEAVE := 0x24000000
else
flasher%flash: DFULEAVE := 0x20030000
eraser%flash: DFULEAVE := 0x20000000
endif

## The eraser flash requires `-a` option
$(OUTPUT_DIRECTORY)/eraser.flash: $(OUTPUT_DIRECTORY)/eraser.dfu
	$(call rule_label,FLASH)
	$(DFU) -D $< && \
	$(DFU) -a 1 -s $(DFULEAVE):leave

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
	$(PYTHON) tools/device/elf2dfu.py -i $$^ -o $$@ -u $$(USBVIDPID)
)
endef

$(call rule_for_composite_dfu,epsilon_test,bootloader/bootloader.elf kernel/kernel.A%elf kernel/kernel.B%elf userland_test.A%elf userland_test.B%elf)
$(call document_other_target,test.<flavors>.dfu)

$(call rule_for_composite_dfu,epsilon,bootloader/bootloader.elf kernel/kernel.A%elf kernel/kernel.B%elf userland.A%elf userland.B%elf)
$(call document_other_target,epsilon.<flavors>.dfu,Composite DFU file made of a bootloader and two kernel&userland)

# Standard dfu-util will refuse to download a .dfu file if it contains sections
# that the device does not expose, e.g. the bootloader section from epsilon.dfu
# when flashing in rescue mode.
$(OUTPUT_DIRECTORY)/epsilon%flash: DFU = $(PYTHON) tools/device/dfu.py
