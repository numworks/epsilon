objs += $(addprefix ion/src/blackbox/, \
  boot.o \
  ion.o \
  display.o \
  events.o \
)

objs += $(addprefix ion/src/shared/, \
  console_line.o \
  console_stdio.o \
  crc32.o \
  events.o \
  power.o \
  random.o \
  dummy/backlight.o \
  dummy/battery.o \
  dummy/events_modifier.o \
  dummy/fcc_id.o \
  dummy/led.o \
  dummy/keyboard.o \
  dummy/serial_number.o \
  dummy/usb.o \
)

ion/src/shared/log_printf.o: SFLAGS=-Iion/include
ion/src/shared/console_stdio.o: SFLAGS=-Iion/include
ion/src/shared/events_stdin.o: SFLAGS=-Iion/include

SFLAGS += `libpng-config --cflags`
LDFLAGS += `libpng-config --ldflags`
