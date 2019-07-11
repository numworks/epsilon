ion_src += $(addprefix ion/src/simulator/, \
  init.cpp \
)

ion_src += $(addprefix ion/src/simulator/boot/, main.cpp)
ion_src += $(addprefix ion/src/simulator/display/, fltklcd.cpp)
ion_src += $(addprefix ion/src/simulator/keyboard/, fltkkbd.cpp)

ion_src += $(addprefix ion/src/shared/, \
  crc32.cpp \
  console_line.cpp \
  console_stdio.cpp \
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

SFLAGS += `fltk-config --cflags`
LDFLAGS += `fltk-config --ldflags`
