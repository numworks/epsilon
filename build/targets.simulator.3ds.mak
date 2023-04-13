
%.smdh: ion/src/simulator/3ds/assets/logo.png
	$(Q) echo "SMDH    $(notdir $@)"
	$(Q) smdhtool --create "Upsilon" "A Numworks in your 3DS!" "Numworks" $< $@

$(BUILD_DIR)/%.3dsx: $(BUILD_DIR)/%.elf $(BUILD_DIR)/%.smdh
	$(Q) echo "3DSX    $(notdir $@)"
	$(Q) 3dsxtool $< $@ --smdh=$(word 2,$^)

cia_version_tuple := $(subst ., ,$(OMEGA_VERSION))

cia_version_major := $(word 1,$(cia_version_tuple))
cia_version_minor := $(word 2,$(cia_version_tuple))
cia_version_micro := $(word 3,$(cia_version_tuple))


$(BUILD_DIR)/%.cxi: $(BUILD_DIR)/%.elf ion/src/simulator/3ds/assets/app.rsf ion/src/simulator/3ds/assets/banner.bnr ion/src/simulator/3ds/assets/icon.icn
	$(Q) echo "CXI     $(notdir $@)"
	$(Q) makerom -o $@ -rsf ion/src/simulator/3ds/assets/app.rsf -target t -elf $< -icon ion/src/simulator/3ds/assets/icon.icn -banner ion/src/simulator/3ds/assets/banner.bnr -desc app:4

$(BUILD_DIR)/%.cia: $(BUILD_DIR)/%.cxi
	$(Q) echo "CIA     $(notdir $@)"
	$(Q) makerom -f cia -o $@ -target t -i $<:0:0
