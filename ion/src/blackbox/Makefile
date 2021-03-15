ion_src += $(addprefix ion/src/blackbox/, \
  boot.cpp \
  ion.cpp \
  display.cpp \
  events.cpp \
)

ion_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  console_stdio.cpp \
  crc32.cpp \
  events.cpp \
  power.cpp \
  random.cpp \
  timing.cpp \
  dummy/backlight.cpp \
  dummy/battery.cpp \
  dummy/rtc.cpp \
  dummy/display.cpp \
  dummy/events_modifier.cpp \
  dummy/exam_mode.cpp \
  dummy/fcc_id.cpp \
  dummy/led.cpp \
  dummy/keyboard.cpp \
  dummy/serial_number.cpp \
  dummy/stack.cpp \
  dummy/usb.cpp \
)

$(call object_for,ion/src/shared/log_printf.cpp): SFLAGS=-Iion/include
$(call object_for,ion/src/shared/console_stdio.cpp): SFLAGS=-Iion/include
$(call object_for,ion/src/shared/events_stdin.cpp): SFLAGS=-Iion/include

SFLAGS += `libpng-config --cflags`
LDFLAGS += `libpng-config --ldflags`
