.PHONY: scenario_logger
scenario_logger:
	$(Q) make clean && make -j8 DEBUG=1 PLATFORM=blackbox epsilon.bin
	$(Q) cp epsilon.bin epsilon_scenario_logger.bin
	@echo "Run ./epsilon_scenario_logger.bin --logAfter 0 < scenario.esc to log a scenario"

.PHONY: scenario_creator
scenario_creator:
	$(Q) make clean && make -j8 DEBUG=1 ESCHER_LOG_EVENTS_BINARY=1 PLATFORM=simulator epsilon.elf
	$(Q) cp epsilon.elf epsilon_scenario_creator.elf
	@echo "Run ./epsilon_scenario_creator.elf > scenario.esc to create a scenario"
