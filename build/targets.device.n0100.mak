.PHONY: $(BUILD_DIR)/epsilon.dfu
$(BUILD_DIR)/epsilon.dfu: | $(BUILD_DIR)/.
	$(MAKE) FIRMWARE_COMPONENT=kernel kernel.elf
	$(MAKE) FIRMWARE_COMPONENT=userland userland.elf
	$(PYTHON) build/device/elf2dfu.py $(DFUFLAGS) -i \
	  $(subst epsilon,kernel,$(BUILD_DIR)/kernel.elf) \
	  $(subst epsilon,userland,$(BUILD_DIR)/userland.elf) \
	  -o $@

.PHONY: $(BUILD_DIR)/test.dfu
$(BUILD_DIR)/test.dfu: | $(BUILD_DIR)/.
	$(MAKE) FIRMWARE_COMPONENT=kernel kernel.elf
	$(MAKE) FIRMWARE_COMPONENT=userland userland.test.elf
	$(PYTHON) build/device/elf2dfu.py $(DFUFLAGS) -i \
	  $(subst test,kernel,$(BUILD_DIR))/kernel.elf \
	  $(subst test,userland,$(BUILD_DIR))/userland.test.elf \
	  -o $@

.PHONY: %_flash
%_flash: $(BUILD_DIR)/%.dfu
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press the RESET button the back of your device."
	$(Q) until $(PYTHON) build/device/dfu.py -l | grep -E "0483:a291|0483:df11" > /dev/null 2>&1; do sleep 2;done
	$(Q) $(PYTHON) build/device/dfu.py -m -u $<
