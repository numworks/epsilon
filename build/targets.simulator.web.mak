$(BUILD_DIR)/test.js: EMSCRIPTEN_MODULARIZE = 0

HANDY_TARGETS += htmlpack htmlpack.official
HANDY_TARGETS_EXTENSIONS += zip

htmlpack_targets = .\
  .official. \

define rule_htmlpack
$$(BUILD_DIR)/htmlpack$(1)zip: $$(addprefix $$(BUILD_DIR)/ion/src/simulator/web/,calculator.html calculator.css) $$(BUILD_DIR)/epsilon$(1)js ion/src/simulator/web/calculator.js
endef

$(foreach target,$(htmlpack_targets),$(eval $(call rule_htmlpack,$(target))))
