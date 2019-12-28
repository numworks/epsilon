# Makefile belows should augment $(ion_device_src)
include ion/src/device/shared/Makefile
include ion/src/device/bench/Makefile
include ion/src/device/flasher/Makefile
include ion/src/device/$(MODEL)/Makefile

$(call object_for,ion/src/shared/platform_info.cpp): SFLAGS += -DHEADER_SECTION="__attribute__((section(\".header\")))"

ion_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  events_keyboard.cpp \
  events_modifier.cpp \
)

# If you need to benchmark execution, you can replace events_keyboard with
# events_benchmark.
# If you need to profile execution, you can replace events_keyboard with
# events_replay.o and dummy/events_modifier.o

ION_DEVICE_SFLAGS = -Iion/src/device/$(MODEL) -Iion/src/device/shared

$(call object_for,$(sort $(ion_device_src) $(dfu_src) $(ion_target_device_flasher_light_src) $(ion_target_device_flasher_verbose_src) $(usb_src) $(ion_target_device_bench_src) $(ion_device_dfu_xip_src) $(ion_device_dfu_relocated_src) $(ion_console_uart_src))): SFLAGS += $(ION_DEVICE_SFLAGS)
ion_src += $(ion_device_src)
