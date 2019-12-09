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
  dummy/exam_mode.cpp \
  dummy/fcc_id.cpp \
  dummy/led.cpp \
  dummy/serial_number.cpp \
  dummy/stack.cpp \
  dummy/usb.cpp \
)

ion_simulator_sdl_src += $(addprefix ion/src/simulator/shared/, \
  display.cpp \
  events_keyboard.cpp \
  framebuffer_base.cpp \
  keyboard_sdl.cpp \
  main_sdl.cpp \
  layout.cpp \
)

ion_simulator_headless_src += $(addprefix ion/src/simulator/shared/, \
  events_stdin.cpp \
  framebuffer_base.cpp \
  framebuffer_png.cpp \
  keyboard_dummy.cpp \
  main_headless.cpp \
)

ion_console_stdio_src = ion/src/simulator/shared/console_stdio.cpp

include ion/src/simulator/$(TARGET)/Makefile
include ion/src/simulator/external/Makefile
