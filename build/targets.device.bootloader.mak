
epsilon_flavors_bootloader = $(foreach floavor,$(epsilon_flavors),$(floavor).A $(floavor).B)

define rule_for_epsilon_flavor_bootloader
$$(BUILD_DIR)/epsilon.$(1).A.$$(EXE): $$(call flavored_object_for,$$(epsilon_src),$(1))
$$(BUILD_DIR)/epsilon.$(1).A.$$(EXE): LDSCRIPT = ion/src/device/bootloader/bootloader.A.ld
$$(BUILD_DIR)/epsilon.$(1).B.$$(EXE): $$(call flavored_object_for,$$(epsilon_src),$(1))
$$(BUILD_DIR)/epsilon.$(1).B.$$(EXE): LDSCRIPT = ion/src/device/bootloader/bootloader.B.ld
endef

$(BUILD_DIR)/epsilon.A.$(EXE): $(call flavored_object_for,$(epsilon_src))
$(BUILD_DIR)/epsilon.A.$(EXE): LDSCRIPT = ion/src/device/bootloader/bootloader.A.ld

$(BUILD_DIR)/epsilon.B.$(EXE): $(call flavored_object_for,$(epsilon_src))
$(BUILD_DIR)/epsilon.B.$(EXE): LDSCRIPT = ion/src/device/bootloader/bootloader.B.ld


$(foreach flavor,$(epsilon_flavors),$(eval $(call rule_for_epsilon_flavor_bootloader,$(flavor))))


HANDY_TARGETS = $(foreach flavor,$(epsilon_flavors_bootloader),epsilon.$(flavor))
HANDY_TARGETS += epsilon.A epsilon.B


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
	$(Q) $(OBJCOPY) -O binary -R .slot_info $< $(basename $<).external.bin
	@echo "Padding $(basename $<).external.bin"
	$(Q) printf "\xFF\xFF\xFF\xFF" >> $(basename $<).external.bin

.PHONY: binpack
binpack: $(BUILD_DIR)/epsilon.onboarding.A.two_binaries $(BUILD_DIR)/epsilon.onboarding.B.two_binaries
	rm -rf $(BUILD_DIR)/binpack
	mkdir -p $(BUILD_DIR)/binpack
	cp $(BUILD_DIR)/epsilon.onboarding.A.external.bin $(BUILD_DIR)/epsilon.onboarding.B.external.bin $(BUILD_DIR)/binpack
	cd $(BUILD_DIR) && for binary in epsilon.onboarding.A.external.bin epsilon.onboarding.B.external.bin; do shasum -a 256 -b binpack/$${binary} > binpack/$${binary}.sha256;done
	cd $(BUILD_DIR) && tar cvfz binpack-$(MODEL)-`git rev-parse HEAD | head -c 7`.tgz binpack/*
