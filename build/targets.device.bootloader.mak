HANDY_TARGETS += test.external_flash.write test.external_flash.read

$(BUILD_DIR)/test.external_flash.%.$(EXE): LDSCRIPT = ion/test/device/n0110/external_flash_tests.ld
test_external_flash_src = $(ion_src) $(liba_src) $(libaxx_src) $(kandinsky_src) $(poincare_src) $(ion_device_dfu_relogated_src) $(runner_src)
$(BUILD_DIR)/test.external_flash.read.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_read_symbols.o $(call object_for,$(test_external_flash_src) $(test_ion_external_flash_read_src))
$(BUILD_DIR)/test.external_flash.write.$(EXE): $(BUILD_DIR)/quiz/src/test_ion_external_flash_write_symbols.o $(call object_for,$(test_external_flash_src) $(test_ion_external_flash_write_src))

.PHONY: %_flash
%_flash: $(BUILD_DIR)/%.dfu
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press at the same time the 6 key and the RESET"
	@echo "        button on the back of your device."
	$(Q) until $(PYTHON) build/device/dfu.py -l | grep -E "0483:a291|0483:df11" > /dev/null 2>&1; do sleep 2;done
	$(Q) $(PYTHON) build/device/dfu.py -u $(word 1,$^)

.PHONY: %.two_binaries
%.two_binaries: %.elf
	@echo "Building an external binary for     $<"
	$(Q) $(OBJCOPY) -O binary $< $(basename $<).external.bin
	@echo "Padding $(basename $<).external.bin"
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(basename $<).external.bin

.PHONY: binpack
binpack: $(BUILD_DIR)/epsilon.onboarding.two_binaries
	rm -rf $(BUILD_DIR)/binpack
	mkdir -p $(BUILD_DIR)/binpack
	cp $(BUILD_DIR)/epsilon.onboarding.external.bin $(BUILD_DIR)/binpack
	cd $(BUILD_DIR) && for binary in epsilon.onboarding.external.bin; do shasum -a 256 -b binpack/$${binary} > binpack/$${binary}.sha256;done
	cd $(BUILD_DIR) && tar cvfz binpack-$(MODEL)-`git rev-parse HEAD | head -c 7`.tgz binpack/*
