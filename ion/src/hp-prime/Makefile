objs += $(addprefix ion/src/hp-prime/boot/, rt0.o)

objs += $(addprefix ion/src/shared/, \
  crc32.o \
  events_modifier.o \
  power.o \
  dummy/backlight.o \
  dummy/battery.o \
  dummy/fcc_id.o \
  dummy/led.o \
  dummy/random.o \
  dummy/serial_number.o \
  dummy/usb.o \
)

objs += $(addprefix ion/src/hp-prime/, \
  device.o\
  display.o\
  events.o\
  keyboard.o\
)

LDFLAGS += -T ion/src/hp-prime/boot/flash.ld