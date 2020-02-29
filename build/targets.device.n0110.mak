HANDY_TARGETS += test.external_flash.write test.external_flash.read

$(BUILD_DIR)/test.external_flash.%.$(EXE): LDSCRIPT = ion/test/device/n0110/external_flash_tests.ld
test_external_flash_src = $(ion_src) $(liba_src) $(libaxx_src) $(kandinsky_src) $(poincare_src) $(ion_device_dfu_relogated_src) $(runner_src)
$(BUILD_DIR)/test.external_flash.read.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_read_symbols.o $(call object_for,$(test_external_flash_src) $(test_ion_external_flash_read_src))
$(BUILD_DIR)/test.external_flash.write.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_write_symbols.o $(call object_for,$(test_external_flash_src) $(test_ion_external_flash_write_src))

.PHONY: %_flash
%_flash: $(BUILD_DIR)/%.dfu $(BUILD_DIR)/flasher.light.dfu
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press at the same time the 6 key and the RESET"
	@echo "        button on the back of your device."
	$(Q) until $(PYTHON) build/device/dfu.py -l | grep -E "0483:a291|0483:df11" > /dev/null 2>&1; do sleep 2;done
	$(eval DFU_SLAVE := $(shell $(PYTHON) build/device/dfu.py -l | grep -E "0483:a291|0483:df11"))
	$(Q) if [[ "$(DFU_SLAVE)" == *"0483:df11"* ]]; \
	  then \
	    $(PYTHON) build/device/dfu.py -u $(word 2,$^); \
	    sleep 2; \
	fi
	$(Q) $(PYTHON) build/device/dfu.py -u $(word 1,$^)

.PHONY: %.two_binaries
%.two_binaries: %.elf
	@echo "Building an internal and an external binary for     $<"
	$(Q) $(OBJCOPY) -O binary -j .text.external -j .rodata.external -j .exam_mode_buffer $(BUILD_DIR)/$< $(BUILD_DIR)/$(basename $<).external.bin
	$(Q) $(OBJCOPY) -O binary -R .text.external -R .rodata.external -R .exam_mode_buffer $(BUILD_DIR)/$< $(BUILD_DIR)/$(basename $<).internal.bin
	@echo "Padding $(basename $<).external.bin and $(basename $<).internal.bin"
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(BUILD_DIR)/$(basename $<).external.bin
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(BUILD_DIR)/$(basename $<).internal.bin

.PHONY: binpack
binpack:
	rm -rf output/binpack
	mkdir -p output/binpack
	$(MAKE) clean
	$(MAKE) $(BUILD_DIR)/flasher.light.bin
	cp $(BUILD_DIR)/flasher.light.bin output/binpack
	$(MAKE) $(BUILD_DIR)/bench.flash.bin
	$(MAKE) $(BUILD_DIR)/bench.ram.bin
	cp $(BUILD_DIR)/bench.ram.bin $(BUILD_DIR)/bench.flash.bin output/binpack
	$(MAKE) epsilon.official.onboarding.update.two_binaries
	cp $(BUILD_DIR)/epsilon.official.onboarding.update.internal.bin $(BUILD_DIR)/epsilon.official.onboarding.update.external.bin output/binpack
	$(MAKE) clean
	cd output && for binary in flasher.light.bin bench.flash.bin bench.ram.bin epsilon.official.onboarding.update.internal.bin epsilon.official.onboarding.update.external.bin; do shasum -a 256 -b binpack/$${binary} > binpack/$${binary}.sha256;done
	cd output && tar cvfz binpack-`git rev-parse HEAD | head -c 7`.tgz binpack
	rm -rf output/binpack
