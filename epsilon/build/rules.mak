.PHONY: custom_userland.flash
custom_userland.flash:
	@echo "Finding connected device model and inactive slot."
	MODEL=$$($(PYTHON) tools/device/dfu.py --model) && \
	SLOT=$$($(PYTHON) tools/device/dfu.py --inactive_slot) && \
	DFULEAVE=$$(if [[ "$$SLOT" == "A" ]]; then echo "0x90010000"; elif [[ "$$SLOT" == "B" ]]; then echo "0x90410000"; fi) && \
	echo "DETECTED MODEL\t$$MODEL" && \
	echo "LEAVE AT\t$$DFULEAVE" && \
	$(MAKE) MODEL=$$MODEL userland.$$SLOT.dfu && \
	$(PYTHON) tools/device/dfu.py -s $$DFULEAVE:leave -D output/release/$$MODEL/userland.$$SLOT.dfu

$(call document_other_target,custom_userland.flash,Write firmware as a custom userland to a connected device in its inactive slot. APP_VERSION must match the official version of the device's official software.)
