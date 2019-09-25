$(BUILD_DIR)/epsilon.packed.js: LDFLAGS += --memory-init-file 0
$(BUILD_DIR)/epsilon.packed.js: $(call object_for,$(all_epsilon_default_src))

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
