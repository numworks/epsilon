# TODO
ion_src += $(addprefix ion/src/shared/, \
  crc32.cpp \
  events.cpp \
  events_keyboard.cpp \
  events_modifier.cpp \
  power.cpp \
  random.cpp \
  timing.cpp \
  dummy/backlight.cpp \
  dummy/battery.cpp \
  dummy/display.cpp \
  dummy/fcc_id.cpp \
  dummy/led.cpp \
  dummy/serial_number.cpp \
  dummy/stack.cpp \
  dummy/usb.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  display.cpp \
  events_keyboard_platform.cpp \
  keyboard.cpp \
  main.cpp \
  layout.cpp \
)

include ion/src/simulator/$(TARGET)/Makefile
include ion/src/simulator/external/Makefile
