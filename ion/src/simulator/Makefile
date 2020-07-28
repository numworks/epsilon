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

$(eval $(call depends_on_image,ion/src/simulator/shared/layout.cpp,ion/src/simulator/assets/key_layouts/horizontal_arrow.png))
$(eval $(call depends_on_image,ion/src/simulator/shared/layout.cpp,ion/src/simulator/assets/key_layouts/large_squircle.png))
$(eval $(call depends_on_image,ion/src/simulator/shared/layout.cpp,ion/src/simulator/assets/key_layouts/round.png))
$(eval $(call depends_on_image,ion/src/simulator/shared/layout.cpp,ion/src/simulator/assets/key_layouts/small_squircle.png))
$(eval $(call depends_on_image,ion/src/simulator/shared/layout.cpp,ion/src/simulator/assets/key_layouts/vertical_arrow.png))

include ion/src/simulator/$(TARGET)/Makefile
include ion/src/simulator/external/Makefile
