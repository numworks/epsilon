HANDY_TARGETS += bench.flash bench.ram

bench_src = $(ion_src) $(ion_device_bench_src) $(liba_src) $(libaxx_src) $(kandinsky_src) $(poincare_src)
$(BUILD_DIR)/bench.ram.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart usbxip)
$(BUILD_DIR)/bench.flash.$(EXE): $(call flavored_object_for,$(bench_src),consoleuart usbxip)

$(BUILD_DIR)/bench.%.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared -Lion/src/$(PLATFORM)/$(MODEL)/shared -Lion/src/$(PLATFORM)/$(MODEL)/bench
$(BUILD_DIR)/bench.flash.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/bench/flash.ld
$(BUILD_DIR)/bench.ram.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/$(MODEL)/bench/ram.ld
$(BUILD_DIR)/bench.%.$(EXE): LDDEPS += $(LDSCRIPT) ion/src/$(PLATFORM)/shared/shared_config_flash.ld ion/src/$(PLATFORM)/$(MODEL)/shared/config_flash.ld
