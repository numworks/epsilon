$(BUILD_DIR)/ion/src/shared/tools/event_%: ion/src/shared/tools/event_%.cpp ion/src/shared/events.cpp
	@echo "HOSTCXX $@"
	@$(HOSTCXX) -std=c++11 -Iion/include $^ -o $@
