PHONY: %_run
%_run: $(BUILD_DIR)/%.$(EXE)
	$(call rule_label,EXE)
	$(Q) ./$^
