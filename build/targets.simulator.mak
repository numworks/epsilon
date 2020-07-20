# Headless targets
$(eval $(call rule_for_epsilon_flavor,headless))
HANDY_TARGETS += epsilon.headless

$(BUILD_DIR)/test.headless.$(EXE): $(call flavored_object_for,$(test_runner_src),headless)
HANDY_TARGETS += test.headless

-include build/targets.simulator.$(TARGET).mak
