# The following lines allow us to use our own SDL_config.h

# First, make sure an error is raised if we ever use the standard SDL_config.h
SFLAGS += -DUSING_GENERATED_CONFIG_H
# Then use our very own include dir if either SDL.h or SDL_config.h are included
# Note that this one will appear before ion/src/simulator/external/sdl/include
# which is added later on in the SDL Makefile
SFLAGS += -Iion/src/simulator/linux/include

ion_src += $(addprefix ion/src/simulator/linux/, \
  images.cpp \
  language.cpp \
  background.s \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/callback.cpp \
  collect_registers_x86_64.s \
  collect_registers.cpp \
)

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/shared/dummy/telemetry_init.cpp
ion_src += ion/src/shared/telemetry_console.cpp
endif

LDFLAGS += -ljpeg
