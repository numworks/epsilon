# TODO
objs += $(addprefix ion/src/shared/, \
  crc32.o \
  crc32_padded.o \
  events.o \
  events_modifier.o \
  power.o \
  random.o \
  timing.o \
  dummy/backlight.o \
  dummy/battery.o \
  dummy/fcc_id.o \
  dummy/led.o \
  dummy/serial_number.o \
  dummy/stack.o \
  dummy/usb.o \
)

objs += $(addprefix ion/src/sdl/, \
  display.o \
  events.o \
  main.o \
  keyboard.o \
  layout.o \
)

objs += $(addprefix ion/src/sdl/android/, \
  images.o \
)

SFLAGS += -I/Users/romain/Sources/SDL/include

include ion/src/sdl/external/Makefile
