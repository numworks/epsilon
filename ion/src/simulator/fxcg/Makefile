
ion_src += $(addprefix ion/src/simulator/fxcg/, \
  main.cpp \
  clipboard.cpp \
  display.cpp \
  framebuffer.cpp \
  telemetry_init.cpp \
  keyboard.cpp \
  events_keyboard.cpp \
  events.cpp \
  timing.cpp \
  console.cpp \
  backlight.cpp \
  power.cpp \
  menuHandler.cpp \
)

liba_src += $(addprefix liba/src/, \
  strlcat.c \
  strlcpy.c \
)

ion_src += ion/src/shared/collect_registers.cpp

sdl_simu_needs_to_be_removed += $(addprefix ion/src/simulator/shared/, \
  main.cpp \
  clipboard.cpp \
  display.cpp \
  framebuffer.cpp \
  keyboard.cpp \
  events_keyboard.cpp \
  events_platform.cpp \
  events.cpp \
  layout.cpp \
  actions.cpp \
  window.cpp \
  timing.cpp \
  console.cpp \
)

sdl_simu_needs_to_be_removed += $(addprefix ion/src/shared/dummy/, \
  backlight.cpp \
  power.cpp \
)

#sdl_simu_needs_to_be_removed += $(addprefix ion/src/simulator/shared/dummy/, \
#  display.cpp \
#  led.cpp \
#  usb.cpp \
#  battery.cpp \
#  store_script.cpp \
#)

# Remove the dummy diaplay (re-implemented) and the SDL simulator stuff.
ion_src := $(filter-out $(sdl_simu_needs_to_be_removed),$(ion_src))

SFLAGS := $(filter-out -Iion/src/simulator/external/sdl/include,$(SFLAGS))

SFLAGS += -DFXCG50 -DTARGET_FXCG50 -m4-nofpu -mb -ffreestanding -nostdlib -Wa,--dsp -fstrict-volatile-bitfields -g -Os
LDFLAGS += -nostdlib -Wl,--no-warn-rwx-segments -lgint-cg -lc -lgint-cg -lc -lgcc -lopenlibm -lstdc++ -lgcc

ifdef FASTLOAD
  LDFLAGS += -T fxcg50_fastload.ld
else
  LDFLAGS += -T fxcg50.ld
endif
