src += $(addprefix ion/src/simulator/, \
  init.cpp \
)

src += $(addprefix ion/src/simulator/boot/, main.cpp)
src += $(addprefix ion/src/simulator/display/, fltklcd.cpp)
src += $(addprefix ion/src/simulator/keyboard/, fltkkbd.cpp)

src += $(addprefix ion/src/shared/, \
  crc32.cpp \
  crc32_padded.cpp \
  console_line.cpp \
  console_stdio.cpp \
  events_modifier.cpp \
  power.cpp \
  random.cpp \
  timing.cpp \
  dummy/backlight.cpp \
  dummy/battery.cpp \
  dummy/fcc_id.cpp \
  dummy/led.cpp \
  dummy/serial_number.cpp \
  dummy/stack.cpp \
  dummy/usb.cpp \
  dummy/rpi.o \
)

SFLAGS += `fltk-config --cflags`
LDFLAGS += `fltk-config --ldflags`
