
%.smdh: ion/src/simulator/3ds/assets/logo.png
	$(Q) echo "SMDH    $(notdir $@)"
	$(Q) smdhtool --create "Epsilon" "A Numworks in your 3DS!" "Numworks" ion/src/simulator/3ds/assets/logo.png $@

$(BUILD_DIR)/%.3dsx: $(BUILD_DIR)/%.elf $(BUILD_DIR)/%.smdh
	$(Q) echo "3DSX    $(notdir $@)"
	$(Q) 3dsxtool $< $@ --smdh=$(word 2,$^)

