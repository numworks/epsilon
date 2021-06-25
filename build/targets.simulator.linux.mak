.PHONY: %_run
%_run: $(BUILD_DIR)/%.$(EXE)
	$(call rule_label,EXE)
	$(Q) ./$^

# Reload the simulator
reload: default
	$(Q) pgrep Epsilon && pkill -USR1 Epsilon || echo "No Epsilon executable running"
	$(Q) ./$(BUILD_DIR)/$(firstword $(HANDY_TARGETS)).$(firstword $(HANDY_TARGETS_EXTENSIONS)) --load-state-file /tmp/numworks.reload.nws
