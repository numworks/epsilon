ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/backlight.cpp \
  dummy/battery.cpp \
  dummy/display.cpp \
  dummy/exam_mode.cpp \
  dummy/fcc_id.cpp \
  dummy/led.cpp \
  dummy/serial_number.cpp \
  dummy/stack.cpp \
  dummy/timing_msleep.cpp:+headless \
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
  main.cpp \
  power.cpp \
  random.cpp \
  timing.cpp \
  timing_msleep.cpp:-headless \
  window_headless.cpp:+headless \
  window_sdl.cpp:-headless \
)

ion_simulator_assets = background.jpg horizontal_arrow.png vertical_arrow.png round.png small_squircle.png large_squircle.png
ion_simulator_assets_paths = $(add_prefix ion/src/simulator/assets/,$(ion_simulator_assets))

include ion/src/simulator/$(TARGET)/Makefile
include ion/src/simulator/external/Makefile
