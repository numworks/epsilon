PHONY: %_run
t %_run: $(BUILD_DIR)/%.$(EXE)
	$(call rule_label,EXE)
	$(Q) ./$^
