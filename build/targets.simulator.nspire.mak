$(BUILD_DIR)/%.tns: $(BUILD_DIR)/%.elf
# comment one of these lines. For B&W old nspire, do not use the compress option
	genzehn --compress --input  $(BUILD_DIR)/epsilon.elf --output upsilon.tns --name "upsilon"  --uses-lcd-blit true
#	genzehn --input  $(BUILD_DIR)/epsilon.elf --output upsilon.tns --name "upsilon"  --uses-lcd-blit true
	genzehn --info --input upsilon.tns
	firebird-send upsilon.tns /ndless	
