# Makefile belows should augment $(ion_device_src)
include ion/src/device/shared/Makefile
include ion/src/device/bench/Makefile
include ion/src/device/flasher/Makefile
include ion/src/device/$(MODEL)/Makefile

$(call object_for,ion/src/shared/platform_info.cpp): SFLAGS += -DHEADER_SECTION="__attribute__((section(\".header\")))"

src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  events_keyboard.cpp \
  events_modifier.cpp \
)

# If you need to benchmark execution, you can replace events_keyboard with
# events_benchmark.
# If you need to profile execution, you can replace events_keyboard with
# events_replay.o and dummy/events_modifier.o

ION_DEVICE_SFLAGS = -Iion/src/device/$(MODEL) -Iion/src/device/shared

$(call object_for,$(ion_device_src) $(dfu_src) $(flasher_src) $(usb_src) $(bench_src) $(ion_device_dfu_xip_src) $(ion_device_dfu_relocated_src)): SFLAGS += $(ION_DEVICE_SFLAGS)
src += $(ion_device_src)

# When using the register.h C++ file in production mode, we expect the compiler
# to completely inline all bit manipulations. For some reason, if we build using
# the -Os optimization flag, GCC doesn't inline everything and and ends up
# emitting calls to aeabi_llsl for 64-bits registers. This is very sub-optimal
# so we're enforcing -O3 for this specific file.

ifneq ($(DEBUG),1)
ifneq ($(COMPILER),llvm)
$(BUILD_DIR)/ion/src/device/led.o: SFLAGS+=-O3
$(BUILD_DIR)/ion/src/device/console.o: SFLAGS+=-O3
$(BUILD_DIR)/ion/src/device/display.o: SFLAGS+=-O3
$(BUILD_DIR)/ion/src/device/swd.o: SFLAGS+=-O3
endif
endif