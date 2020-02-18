.PHONY: scenario_logger
scenario_logger:
	$(Q) $(MAKE) PLATFORM=simulator clean && $(MAKE) DEBUG=1 ARCH=x86_64 PLATFORM=simulator epsilon.headless.bin
	$(Q) cp output/debug/simulator/macos/x86_64/epsilon.headless.bin epsilon_scenario_logger.bin
	@echo "Run ./epsilon_scenario_logger.bin --logAfter 0 < scenario.esc to log a scenario"

.PHONY: scenario_creator
scenario_creator:
	$(Q) $(MAKE) PLATFORM=simulator clean && $(MAKE) DEBUG=1 ESCHER_LOG_EVENTS_BINARY=1 PLATFORM=simulator
	$(Q) cp -R output/debug/simulator/macos/app/Payload/Epsilon.app epsilon_scenario_creator.app
	@echo "Run lldb epsilon_scenario_creator.app then process launch -o scenario.esc to create a scenario"
