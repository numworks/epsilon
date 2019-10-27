$(BUILD_DIR)/epsilon.js: EMSCRIPTEN_INIT_FILE = 1

$(BUILD_DIR)/test.headless.js: EMSCRIPTEN_MODULARIZE = 0

$(BUILD_DIR)/epsilon.packed.js: $(call object_for,$(epsilon_src))

.PHONY: workshop_python_emulator
workshop_python_emulator:
	make PLATFORM=simulator TARGET=web clean_for_apps_selection
	make PLATFORM=simulator TARGET=web EPSILON_APPS=code
	make PLATFORM=simulator TARGET=web clean_for_apps_selection

.PHONY: clean_for_apps_selection
clean_for_apps_selection:
	@echo "CLEAN BEFORE CHANGING EPSILON_APPS"
	$(Q) rm -f $(BUILD_DIR)/apps/apps_container_storage.o
	$(Q) rm -f $(BUILD_DIR)/apps/i18n.*
