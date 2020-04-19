
ion_src += $(addprefix ion/src/simulator/3ds/, \
  main.cpp \
  callback.cpp \
  display.cpp \
  framebuffer.cpp \
  language.cpp \
  telemetry_init.cpp \
  keyboard.cpp \
  events_keyboard.cpp \
  driver/display.cpp \
)

sdl_simu_needs_to_be_removed += $(addprefix ion/src/simulator/shared/, \
  dummy/display.cpp \
  display.cpp:-headless \
  events_keyboard.cpp:-headless \
  framebuffer_base.cpp \
  keyboard_sdl.cpp:-headless \
  main_sdl.cpp:-headless \
  layout.cpp:-headless \
)

# Remove the dummy diplay (re-implemented) and the SDL simulator stuff.
ion_src := $(filter-out $(sdl_simu_needs_to_be_removed),$(ion_src))

