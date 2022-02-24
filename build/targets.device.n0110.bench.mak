HANDY_TARGETS += bench.flash bench.ram

bench_src = $(ion_device_bench_src) $(liba_src) $(liba_bench_src) $(libaxx_src) $(kandinsky_src) $(poincare_src)
$(BUILD_DIR)/bench.ram.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart)
$(BUILD_DIR)/bench.flash.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart)

$(BUILD_DIR)/bench.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/$(MODEL)/bench
$(BUILD_DIR)/bench.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/bench/flash.ld
$(BUILD_DIR)/bench.ram.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/bench/ram.ld
