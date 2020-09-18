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
  events.cpp \
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

assets = background.jpg horizontal_arrow.png vertical_arrow.png round.png small_squircle.png large_squircle.png
assets_paths = $(foreach i,$(assets),ion/src/simulator/assets/$(i))

include ion/src/simulator/$(TARGET)/Makefile
include ion/src/simulator/external/Makefile
