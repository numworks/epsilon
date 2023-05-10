$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary -R .bss -R .gint_bss $< $@

$(BUILD_DIR)/%.g3a: $(BUILD_DIR)/%.bin ion/src/simulator/fxcg/assets/icon-uns.png ion/src/simulator/fxcg/assets/icon-sel.png
	$(FXGXA) --g3a --icon-uns=ion/src/simulator/fxcg/assets/icon-uns.png --icon-sel=ion/src/simulator/fxcg/assets/icon-sel.png -n Upsilon $< -o $@
