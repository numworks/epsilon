HANDY_TARGETS += bench.flash bench.ram

bench_src = $(ion_device_bench_src) $(liba_src) $(liba_bench_src) $(libaxx_src) $(kandinsky_src) $(poincare_bench_src)
$(BUILD_DIR)/bench.ram.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart $(MODEL) ram)
$(BUILD_DIR)/bench.flash.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart $(MODEL) flash)

$(BUILD_DIR)/bench.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/bench
$(BUILD_DIR)/bench.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/bench/flash.ld
$(BUILD_DIR)/bench.ram.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/bench/ram.ld
$(BUILD_DIR)/bench.%.$(EXE): LDDEPS += $(LDSCRIPT)
