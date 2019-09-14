# TODO
src += $(addprefix ion/src/shared/, \
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

src += $(addprefix ion/src/sdl/shared/, \
  display.cpp \
  events_keyboard_platform.cpp \
  keyboard.cpp \
  main.cpp \
  layout.cpp \
)

include ion/src/sdl/$(MODEL)/Makefile
include ion/src/sdl/external/Makefile
