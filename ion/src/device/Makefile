include ion/src/device/boot/Makefile
objs += $(addprefix ion/src/device/, \
  battery.o\
  device.o\
  display.o\
  keyboard.o\
  led.o\
  power.o\
)

# When using the register.h C++ file in production mode, we expect the compiler
# to completely inline all bit manipulations. For some reason, if we build using
# the -Os optimization flag, GCC doesn't inline everything and and ends up
# emitting calls to aeabi_llsl for 64-bits registers. This is very sub-optimal
# so we're enforcing -O3 for this specific file.

ifneq ($(DEBUG),1)
ifneq ($(COMPILER),llvm)
ion/src/device/led.o: SFLAGS+=-O3
ion/src/device/display.o: SFLAGS+=-O3
endif
endif

#objs += $(addprefix ion/src/device/keyboard/, keyboard.o)
