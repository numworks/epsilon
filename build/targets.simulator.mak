$(BUILD_DIR)/epsilon.packed.js: LDFLAGS += --memory-init-file 0
$(BUILD_DIR)/epsilon.packed.js: $(call object_for,$(all_epsilon_default_src))
