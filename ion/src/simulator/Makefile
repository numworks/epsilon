ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/backlight.cpp \
  dummy/battery.cpp \
  dummy/display.cpp \
  dummy/exam_mode.cpp \
  dummy/fcc_id.cpp \
  dummy/led.cpp \
  dummy/serial_number.cpp \
  dummy/stack.cpp \
  dummy/usb.cpp \
  console_stdio.cpp:-consoledisplay \
  crc32.cpp \
  display.cpp:-headless \
  events_keyboard.cpp:-headless \
  events_stdin.cpp:+headless \
  framebuffer_base.cpp \
  framebuffer_png.cpp:+headless \
  keyboard_dummy.cpp:+headless \
  keyboard_sdl.cpp:-headless \
  layout.cpp:-headless \
  main_headless.cpp:+headless \
  main_sdl.cpp:-headless \
  power.cpp \
  random.cpp \
  timing.cpp \
)

include ion/src/simulator/$(TARGET)/Makefile
include ion/src/simulator/external/Makefile
