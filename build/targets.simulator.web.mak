$(BUILD_DIR)/test.js: EMSCRIPTEN_MODULARIZE = 0

HANDY_TARGETS += htmlpack
HANDY_TARGETS_EXTENSIONS += zip

$(BUILD_DIR)/htmlpack.zip: $(addprefix $(BUILD_DIR)/ion/src/simulator/web/,calculator.html calculator.css) $(BUILD_DIR)/epsilon.js ion/src/simulator/web/calculator.js
