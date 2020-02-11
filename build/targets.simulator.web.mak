$(BUILD_DIR)/epsilon%packed.js: EMSCRIPTEN_INIT_FILE = 0

$(BUILD_DIR)/test.headless.js: EMSCRIPTEN_MODULARIZE = 0

$(BUILD_DIR)/epsilon.packed.js: $(call object_for,$(epsilon_src))
$(BUILD_DIR)/epsilon.official.packed.js: $(call object_for,$(epsilon_official_src))
