include ion/src/device/boot/Makefile
include ion/src/device/bench/Makefile
include ion/src/device/usb/Makefile

$(call object_for,ion/src/shared/platform_info.cpp): SFLAGS += -DHEADER_SECTION="__attribute__((section(\".header\")))"

src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  crc32_padded.cpp \
  events_keyboard.cpp \
  events_modifier.cpp \
)

# If you need to profile execution, you can replace events_keyboard with
# events_replay.o and dummy/events_modifier.o

src += $(addprefix ion/src/device/, \
  backlight.cpp \
  battery.cpp \
  base64.cpp \
  console.cpp \
  device.cpp \
  display.cpp \
  events_keyboard_platform.cpp \
  flash.cpp \
  keyboard.cpp \
  led.cpp \
  power.cpp \
  rpi.o \
  sd_card.cpp \
  stack.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  wakeup.cpp \
)

# When using the register.h C++ file in production mode, we expect the compiler
# to completely inline all bit manipulations. For some reason, if we build using
# the -Os optimization flag, GCC doesn't inline everything and and ends up
# emitting calls to aeabi_llsl for 64-bits registers. This is very sub-optimal
# so we're enforcing -O3 for this specific file.

ifneq ($(DEBUG),1)
ifneq ($(COMPILER),llvm)
$(BUILD_DIR)/ion/src/device/led.o: SFLAGS+=-O3
$(BUILD_DIR)/ion/src/device/console.o: SFLAGS+=-O3
$(BUILD_DIR)/ion/src/device/display.o: SFLAGS+=-O3
$(BUILD_DIR)/ion/src/device/swd.o: SFLAGS+=-O3
endif
endif

#objs += $(addprefix ion/src/device/keyboard/, keyboard.o)
