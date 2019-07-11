$(BUILD_DIR)/epsilon.packed.js: LDFLAGS += --memory-init-file 0
$(BUILD_DIR)/epsilon.packed.js: $(call object_for,$(all_epsilon_default_src))

$(BUILD_DIR)/simulator.zip: $(BUILD_DIR)/epsilon.packed.js
	@rm -rf $(basename $@)
	@mkdir -p $(basename $@)
	@cp $^ $(basename $@)/epsilon.js
	@cp ion/src/emscripten/background.jpg $(basename $@)/
	@cp ion/src/emscripten/simulator.html $(basename $@)/
	@echo "ZIP     $@"
	@zip -r -9 -j $@ $(basename $@) > /dev/null
	@rm -rf $(basename $@)
