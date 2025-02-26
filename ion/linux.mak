SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  linux/assets.s \
  linux/platform_files.cpp \
  linux/platform_images.cpp \
  linux/platform_language.cpp \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/keyboard_callback.cpp \
  shared/dummy/window_callback.cpp \
  shared/unix/platform_files.cpp \
  shared/circuit_breaker.cpp \
  shared/clipboard_helper_sdl.cpp \
  shared/collect_registers_x86_64.s \
  shared/collect_registers.cpp \
  shared/haptics.cpp \
  shared/journal.cpp \
)

# Make sure the call to shell is not in a recursive variable.
_ion_libjpeg_cflags := $(shell pkg-config libpng libjpeg --cflags)
_ion_libjpeg_libs := $(shell pkg-config libpng libjpeg --libs)

SFLAGS_ion += $(_ion_libjpeg_cflags)
LDFLAGS_ion += $(_ion_libjpeg_libs)

_ion_simulator_files := 1

# Generate simulator assets

_ion_simulator_assets := \
$(addprefix $(PATH_ion)/src/simulator/assets/, \
  horizontal_arrow.png \
  large_squircle.png \
  round.png \
  small_squircle.png \
  vertical_arrow.png \
) \
  $(_ion_simulator_background) \
  $(_ion_simulator_backgrounds_generated)

$(addprefix $(OUTPUT_DIRECTORY)/$(PATH_ion)/src/simulator/linux/,assets.s platform_images.h): $(_ion_simulator_assets) | $$(@D)/.
	$(call rule_label,INCBIN)
	$(PYTHON) $(PATH_ion)/src/simulator/linux/incbin.py $^ -o $@

$(call all_objects_for,$(PATH_ion)/src/simulator/linux/platform_images.cpp): $(OUTPUT_DIRECTORY)/$(PATH_ion)/src/simulator/linux/platform_images.h
