$(BUILD_DIR)/test.js: EMSCRIPTEN_MODULARIZE = 0

HANDY_TARGETS += htmlpack
HANDY_TARGETS_EXTENSIONS += zip html

htmlpack_targets = .\

define rule_htmlpack
$$(BUILD_DIR)/htmlpack$(1)zip: $$(addprefix $$(BUILD_DIR)/ion/src/simulator/web/,calculator.html calculator.css) $$(BUILD_DIR)/epsilon$(1)js ion/src/simulator/web/calculator.js ion/src/simulator/assets/background.webp $(BUILD_DIR)ion/src/simulator/assets/background.jpg
endef

$(foreach target,$(htmlpack_targets),$(eval $(call rule_htmlpack,$(target))))

$(BUILD_DIR)/epsilon%html: $(BUILD_DIR)/epsilon%js $(BUILD_DIR)/ion/src/simulator/web/simulator.html output/release/simulator/ion/src/simulator/assets/background.jpg
	@echo "INLINE  $@"
	$(Q) ion/src/simulator/web/inline.py --image $(BUILD_DIR)ion/src/simulator/assets/background.jpg --script $< $(BUILD_DIR)/ion/src/simulator/web/simulator.html > $@
