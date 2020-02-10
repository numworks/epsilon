$(BUILD_DIR)/epsilon%packed.js: EMSCRIPTEN_INIT_FILE = 0

$(BUILD_DIR)/test.headless.js: EMSCRIPTEN_MODULARIZE = 0

$(BUILD_DIR)/epsilon.packed.js: $(call object_for,$(epsilon_src))
$(BUILD_DIR)/epsilon.official.packed.js: $(call object_for,$(epsilon_official_src))

.PHONY: clean_for_apps_selection
clean_for_apps_selection:
	@echo "CLEAN BEFORE CHANGING EPSILON_APPS"
	$(Q) rm -f $(BUILD_DIR)/apps/apps_container_storage.o
	$(Q) rm -f $(BUILD_DIR)/apps/i18n.*
