HANDY_TARGETS += bench.flash bench.ram

bench_src = $(ion_device_bench_src) $(liba_src) $(libaxx_src) $(kandinsky_src) $(poincare_bench_src)
$(BUILD_DIR)/bench.ram.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart $(MODEL) ram)
$(BUILD_DIR)/bench.flash.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart $(MODEL) flash) $(LDDEPS)

$(BUILD_DIR)/bench.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-core/device/bench
$(BUILD_DIR)/bench.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/epsilon-core/device/shared-core/flash
$(BUILD_DIR)/bench.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/bench/flash.ld
$(BUILD_DIR)/bench.ram.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-core/device/bench/ram.ld
$(BUILD_DIR)/bench.%.$(EXE): LDDEPS += $(LDSCRIPT)
