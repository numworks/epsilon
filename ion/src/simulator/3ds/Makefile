
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

# Remove the dummy diplay (re-implemented)
ion_src := $(filter-out addprefix ion/src/shared/dummy/display.cpp,$(ion_src))
