
ion_src += $(addprefix ion/src/simulator/nspire/, \
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
  k_csdk.c \
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

# Remove the dummy diaplay (re-implemented) and the SDL simulator stuff.
ion_src := $(filter-out $(sdl_simu_needs_to_be_removed),$(ion_src))

SFLAGS := $(filter-out -Iion/src/simulator/external/sdl/include,$(SFLAGS))

SFLAGS +=  -marm -DNSPIRE_NEWLIB -I/Users/parisse/Ndless-r2015/ndless-sdk/ndless/include  -I/Users/parisse/Ndless-r2015/ndless-sdk/toolchain/install/include  -fno-strict-aliasing -I. -I.. -fno-exceptions -ffreestanding -nostdlib -fstrict-volatile-bitfields -g -Os
LDFLAGS += -Wl,--nspireio,--gc-sections -L/Users/parisse/Ndless-r2015/ndless-sdk/lib -L/Users/parisse/Ndless-r2015/ndless-sdk/toolchain/install/lib -nostdlib -lc -lm -lstdc++ -lgcc

