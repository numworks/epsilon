# Headless targets
epsilon_headless_src = $(base_src) $(ion_headless_src) $(apps_default_src)
$(BUILD_DIR)/epsilon.headless.$(EXE): $(call object_for,$(epsilon_headless_src))
$(BUILD_DIR)/epsilon.headless.$(EXE): LDFLAGS += `libpng-config --ldflags`

test_runner_headless_src = $(test_base_src) $(ion_headless_src)
$(BUILD_DIR)/test.headless.$(EXE): $(call object_for,$(test_runner_headless_src))
$(BUILD_DIR)/test.headless.$(EXE): LDFLAGS += `libpng-config --ldflags`

HANDY_TARGETS += epsilon.headless test.headless

-include build/targets.simulator.$(TARGET).mak
