
ion_src += $(addprefix ion/src/simulator/3ds/, \
  main.cpp \
  callback.cpp \
  display.cpp \
  framebuffer.cpp \
  telemetry_init.cpp \
  keyboard.cpp \
  events_keyboard.cpp \
  driver/display.cpp \
  driver/language.cpp \
  driver/led.cpp \
  driver/usb.cpp \
  driver/battery.cpp \
  driver/common.cpp \
)

sdl_simu_needs_to_be_removed += $(addprefix ion/src/simulator/shared/, \
  dummy/display.cpp \
  dummy/led.cpp \
  dummy/usb.cpp \
  dummy/battery.cpp \
  display.cpp:-headless \
  events_keyboard.cpp:-headless \
  framebuffer_base.cpp \
  keyboard_sdl.cpp:-headless \
  main_sdl.cpp:-headless \
  layout.cpp:-headless \
)

# Remove the dummy diplay (re-implemented) and the SDL simulator stuff.
ion_src := $(filter-out $(sdl_simu_needs_to_be_removed),$(ion_src))

