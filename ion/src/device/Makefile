include ion/src/device/boot/Makefile
include ion/src/device/bench/Makefile

ion/src/shared/software_version.o: SFLAGS += -DHEADER_SECTION="__attribute__((section(\".header\")))"

objs += $(addprefix ion/src/shared/, \
  console_line.o \
  events_modifier.o \
)

# If you need to profile execution, you can replace events_keyboard with
# events_replay.o and dummy/events_modifier.o

objs += $(addprefix ion/src/device/, \
  backlight.o \
  battery.o\
  console.o \
  device.o\
  display.o\
  events_keyboard.o\
  fcc_id.o\
  keyboard.o\
  led.o\
  power.o\
  sd_card.o\
  swd.o \
  usb.o \
  wakeup.o \
)

# When using the register.h C++ file in production mode, we expect the compiler
# to completely inline all bit manipulations. For some reason, if we build using
# the -Os optimization flag, GCC doesn't inline everything and and ends up
# emitting calls to aeabi_llsl for 64-bits registers. This is very sub-optimal
# so we're enforcing -O3 for this specific file.

ifneq ($(DEBUG),1)
ifneq ($(COMPILER),llvm)
ion/src/device/led.o: SFLAGS+=-O3
ion/src/device/console.o: SFLAGS+=-O3
ion/src/device/display.o: SFLAGS+=-O3
ion/src/device/swd.o: SFLAGS+=-O3
endif
endif

#objs += $(addprefix ion/src/device/keyboard/, keyboard.o)
