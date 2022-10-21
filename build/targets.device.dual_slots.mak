define add_slot_suffix
$(addsuffix .A,$(1)) $(addsuffix .B,$(1))
endef

HANDY_TARGETS += test.external_flash.write test.external_flash.read

$(BUILD_DIR)/test.external_flash.%.$(EXE): LDSCRIPT = ion/test/device/$(MODEL)/external_flash_tests.ld
test_external_flash_src = $(ion_src) $(liba_src) $(libaxx_src) $(kandinsky_src) $(poincare_src) $(ion_device_dfu_relogated_src) $(runner_src)
$(BUILD_DIR)/test.external_flash.read.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_read_symbols.o $(call object_for,$(test_external_flash_src) $(test_ion_external_flash_read_src))
$(BUILD_DIR)/test.external_flash.write.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_write_symbols.o $(call object_for,$(test_external_flash_src) $(test_ion_external_flash_write_src))

# TODO: clean the BUILD_DIR definition in targets.device.mak to avoid requiring FIRMWARE_COMPONENT to:
# - find the right subfolder 'bootloader', 'kernel' or 'userland'
# - be able to rely on the usual rule for dfu
# - avoid requiring a standalone Makefile
#epsilon.dfu: DFUFLAGS += --signer $(BUILD_DIR)/signer --custom
#$(BUILD_DIR)/epsilon.dfu: $(BUILD_DIR)/userland.A.elf $(BUILD_DIR)/kernel.A.elf $(BUILD_DIR)/userland.B.elf $(BUILD_DIR)/kernel.B.elf $(BUILD_DIR)/signer

$(dfu_targets): DFUFLAGS += --custom

$(dfu_targets): $(BUILD_DIR)/%.dfu: | $(BUILD_DIR)/.
	$(if $(NO_BOOTLOADER),,$(MAKE) FIRMWARE_COMPONENT=bootloader DEBUG=0 bootloader.elf)
	$(MAKE) FIRMWARE_COMPONENT=kernel DEBUG=0 kernel$(TARGET_STEM).A.elf
	$(MAKE) FIRMWARE_COMPONENT=kernel DEBUG=0 kernel$(TARGET_STEM).B.elf
	$(MAKE) FIRMWARE_COMPONENT=userland userland$(TARGET_STEM).A.elf
	$(MAKE) FIRMWARE_COMPONENT=userland userland$(TARGET_STEM).B.elf
	$(PYTHON) build/device/elf2dfu.py $(DFUFLAGS) -i \
	  $(if $(NO_BOOTLOADER),,$(subst $(FIRMWARE_COMPONENT),bootloader,$(subst debug,release,$(BUILD_DIR)))/bootloader.elf) \
	  $(subst $(FIRMWARE_COMPONENT),userland,$(BUILD_DIR))/userland$(TARGET_STEM).A.elf \
	  $(subst $(FIRMWARE_COMPONENT),kernel,$(subst debug,release,$(BUILD_DIR)))/kernel$(TARGET_STEM).A.elf \
	  $(subst $(FIRMWARE_COMPONENT),userland,$(BUILD_DIR))/userland$(TARGET_STEM).B.elf \
	  $(subst $(FIRMWARE_COMPONENT),kernel,$(subst debug,release,$(BUILD_DIR)))/kernel$(TARGET_STEM).B.elf \
	  -o $@

.PHONY: %_flash
%_flash: $(BUILD_DIR)/%.dfu
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press at the same time the 6 key and the RESET"
	@echo "        button on the back of your device."
	$(Q) until $(PYTHON) build/device/dfu.py -l | grep -E "0483:a291|0483:df11" > /dev/null 2>&1; do sleep 2;done
	$(eval DFU_SLAVE := $(shell $(PYTHON) build/device/dfu.py -l | grep -E "0483:a291|0483:df11"))
	$(Q) if [[ "$(DFU_SLAVE)" == *"0483:df11"* ]]; \
	  then \
	    $(MAKE) FIRMWARE_COMPONENT=flasher DEBUG=0 flasher.dfu; \
	    $(PYTHON) build/device/dfu.py -s 0x20030000:leave -D $(subst epsilon,flasher,$(BUILD_DIR))/flasher.dfu; \
	    sleep 2; \
	fi
	$(Q) $(PYTHON) build/device/dfu.py -D $(word 1,$^)

.PHONY: %.two_binaries
%.two_binaries: %.elf
	@echo "Building an internal and an external binary for     $<"
	$(Q) $(OBJCOPY) -O binary -j .text.external -j .rodata.external -j .persisting_bytes_buffer $(BUILD_DIR)/$< $(BUILD_DIR)/$(basename $<).external.bin
	$(Q) $(OBJCOPY) -O binary -R .text.external -R .rodata.external -R .persisting_bytes_buffer $(BUILD_DIR)/$< $(BUILD_DIR)/$(basename $<).internal.bin
	@echo "Padding $(basename $<).external.bin and $(basename $<).internal.bin"
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(BUILD_DIR)/$(basename $<).external.bin
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(BUILD_DIR)/$(basename $<).internal.bin

.PHONY: binpack
binpack:
ifndef USE_IN_FACTORY
	@echo "\033[0;31mWARNING\033[0m You are building a binpack."
	@echo "You must specify where this binpack will be used."
	@echo "Please set the USE_IN_FACTORY environment variable to either:"
	@echo "  - 0 for use in diagnostic"
	@echo "  - 1 for use in production"
	@exit -1
endif
	rm -rf output/binpack
	mkdir -p output/binpack
	$(MAKE) clean
	$(MAKE) FIRMWARE_COMPONENT=flasher IN_FACTORY=$(USE_IN_FACTORY) flasher.bin
	cp $(subst binpack,flasher,$(BUILD_DIR))/flasher.bin output/binpack
	$(MAKE) FIRMWARE_COMPONENT=bench IN_FACTORY=$(USE_IN_FACTORY) bench.flash.bin
	$(MAKE) FIRMWARE_COMPONENT=bench IN_FACTORY=$(USE_IN_FACTORY) bench.ram.dfu
	cp $(subst binpack,bench,$(BUILD_DIR)/bench.flash.bin $(BUILD_DIR)/bench.ram.dfu) output/binpack
	$(MAKE) FIRMWARE_COMPONENT=epsilon IN_FACTORY=$(USE_IN_FACTORY) epsilon.onboarding.update.dfu
	cp $(subst binpack,epsilon,$(BUILD_DIR))/epsilon.onboarding.update.dfu output/binpack
	$(MAKE) clean
	cd output/binpack && for binary in flasher.bin bench.flash.bin; do shasum -a 256 -b $${binary} > $${binary}.sha256;done
	cd output && tar cvfz binpack-`git rev-parse HEAD | head -c 7`.tgz binpack
	@echo "Binpack created as \033[0;32moutput/binpack-`git rev-parse HEAD | head -c 7`.tgz\033[0m"
	@echo "\033[0;31mWARNING\033[0m Files bench.ram.dfu and epsilon.official.onboarding.update.dfu need to be signed"
	rm -rf output/binpack
