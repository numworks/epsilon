# The following lines allow us to use our own SDL_config.h

# First, make sure an error is raised if we ever use the standard SDL_config.h
SFLAGS += -DUSING_GENERATED_CONFIG_H
# Then use our very own include dir if either SDL.h or SDL_config.h are included
# Note that this one will appear before ion/src/simulator/external/sdl/include
# which is added later on in the SDL Makefile
SFLAGS += -Iion/src/simulator/web/include

# Only render the screen, not the whole calculator which will be drawn in HTML
SFLAGS += -DEPSILON_SDL_SCREEN_ONLY=1

ion_src += $(addprefix ion/src/simulator/web/, \
  callback.cpp \
  helpers.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/language.cpp \
  dummy/telemetry.cpp \
)
