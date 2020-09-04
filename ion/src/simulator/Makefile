ion_src += $(addprefix ion/src/shared/dummy/, \
  backlight.cpp \
  battery.cpp \
  display.cpp \
  exam_mode.cpp \
  fcc_id.cpp \
  keyboard.cpp:+headless \
  led.cpp \
  power.cpp \
  serial_number.cpp \
  stack.cpp \
  timing_msleep.cpp:+headless \
  usb.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/events_platform.cpp:+headless \
  dummy/keyboard.cpp:+headless \
  console_stdio.cpp:-consoledisplay \
  crc32.cpp \
  display.cpp:-headless \
  events.cpp \
  events_platform.cpp:-headless \
  framebuffer_base.cpp \
  framebuffer_png.cpp:+headless \
  keyboard.cpp:-headless \
  layout.cpp:-headless \
  main.cpp \
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
