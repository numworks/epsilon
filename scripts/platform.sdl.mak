USE_LIBA = 0
EPSILON_ONBOARDING_APP = 0
SFLAGS += -fPIE
EXE = elf

#ifndef SDL_PATH
#  $(error SDL_PATH should point to the SDL2 sources. A recent snapshot is required.)
#endif

#SFLAGS += -I$(SDL_PATH)/include

ifndef MODEL
  $(error MODEL should be defined)
endif

BUILD_DIR := $(BUILD_DIR)/$(MODEL)

include scripts/platform.sdl.$(MODEL).mak
