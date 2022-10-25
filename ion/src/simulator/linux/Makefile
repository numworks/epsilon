ION_SIMULATOR_FILES = 1

# The following lines allow us to use our own SDL_config.h
# First, make sure an error is raised if we ever use the standard SDL_config.h
SFLAGS += -DUSING_GENERATED_CONFIG_H
# Then use our very own include dir if either SDL.h or SDL_config.h are included
# Note that this one will appear before ion/src/simulator/external/sdl/include
# which is added later on in the SDL Makefile
SFLAGS += -Iion/src/simulator/linux/include

ion_src += $(addprefix ion/src/simulator/linux/, \
  assets.s \
  platform_files.cpp \
  platform_images.cpp \
  platform_language.cpp \
)

SFLAGS += $(shell pkg-config libpng libjpeg --cflags)
LDFLAGS += $(shell pkg-config libpng libjpeg --libs)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/haptics_enabled.cpp \
  dummy/keyboard_callback.cpp \
  dummy/window_callback.cpp \
  unix/platform_files.cpp \
  circuit_breaker.cpp \
  clipboard_helper.cpp \
  collect_registers_x86_64.s \
  collect_registers.cpp \
  haptics.cpp \
  journal.cpp \
)

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/shared/dummy/telemetry_init.cpp
ion_src += ion/src/shared/telemetry_console.cpp
endif

$(eval $(call rule_for, \
  INCBIN, \
  ion/src/simulator/linux/assets.s ion/src/simulator/linux/platform_images.h, \
  $(ion_simulator_assets_paths), \
  $$(PYTHON) ion/src/simulator/linux/incbin.py $(foreach asset_path,$(ion_simulator_assets_paths),$(notdir $(asset_path))) -o $$@, \
  global \
))

$(call object_for,ion/src/simulator/linux/platform_images.cpp): $(BUILD_DIR)/ion/src/simulator/linux/platform_images.h

# The header is refered to as <ion/src/simulator/linux/platform_images.h> so make sure it's findable this way
$(call object_for,ion/src/simulator/linux/platform_images.cpp): SFLAGS += -I$(BUILD_DIR)
