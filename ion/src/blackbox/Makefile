objs += $(addprefix ion/src/blackbox/, boot.o ion.o)
objs += $(addprefix ion/src/shared/, \
  console_line.o \
  console_stdio.o \
  crc32.o \
  events.o \
  events_stdin.o \
  power.o \
  dummy/backlight.o \
  dummy/battery.o \
)

ion/src/shared/log_printf.o: SFLAGS=-Iion/include
ion/src/shared/console_stdio.o: SFLAGS=-Iion/include
ion/src/shared/events_stdin.o: SFLAGS=-Iion/include
ion/src/shared/random.o: SFLAGS=-Iion/include
