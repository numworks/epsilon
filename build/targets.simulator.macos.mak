.PHONY: %_run
%_run: $(BUILD_DIR)/%.app
	$(call rule_label,OPEN)
	$(Q) open $^
