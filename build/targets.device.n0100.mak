.PHONY: %_flash
%_flash: $(BUILD_DIR)/%.dfu
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press the RESET button the back of your device."
	$(Q) until $(PYTHON) build/device/dfu.py -l | grep -E "0483:a291|0483:df11" > /dev/null 2>&1; do sleep 2;done
	$(Q) $(PYTHON) build/device/dfu.py -m -u $<

.PHONY: %.two_binaries
%.two_binaries: %.elf
	@echo "Building an internal binary for     $<"
	$(Q) $(OBJCOPY) -O binary -R .text.external -R .rodata.external -R .exam_mode_buffer $< $(basename $<).internal.bin
	@echo "Padding $(basename $<).internal.bin"
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(basename $<).internal.bin

.PHONY: binpack
binpack: $(BUILD_DIR)/epsilon.onboarding.two_binaries
	rm -rf $(BUILD_DIR)/binpack
	mkdir -p $(BUILD_DIR)/binpack
	cp $(BUILD_DIR)/epsilon.onboarding.internal.bin $(BUILD_DIR)/binpack
	cd $(BUILD_DIR) && for binary in epsilon.onboarding.internal.bin; do shasum -a 256 -b binpack/$${binary} > binpack/$${binary}.sha256;done
	cd $(BUILD_DIR) && tar cvfz binpack-$(MODEL)-`git rev-parse HEAD | head -c 7`.tgz binpack/*
