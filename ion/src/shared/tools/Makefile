ion/src/shared/tools/event_%: ion/src/shared/tools/event_%.cpp ion/src/shared/events.cpp
	@echo "HOSTCXX $@"
	@$(HOSTCXX) -std=c++11 -Iion/include -DDEBUG=1 $^ -o $@

products += $(addprefix ion/src/shared/tools/, event_filter event_generator event_parser event_printer)
