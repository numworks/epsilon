objs += $(addprefix ion/src/blackbox/, boot.o ion.o)
objs += $(addprefix ion/src/shared/, \
  crc32.o \
  events.o \
  log_printf.o \
  power.o \
)

ion/src/shared/log_printf.o: SFLAGS=-Iion/include
ion/src/shared/events_stdin.o: SFLAGS=-Iion/include
ion/src/shared/random.o: SFLAGS=-Iion/include
