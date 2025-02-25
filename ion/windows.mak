SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  windows/platform_files.cpp \
  windows/platform_images.cpp \
  windows/platform_language.cpp \
  windows/resources.rc \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/keyboard_callback.cpp \
  shared/dummy/window_callback.cpp \
  shared/circuit_breaker.cpp \
  shared/clipboard_helper_sdl.cpp \
  shared/haptics.cpp \
  shared/journal.cpp \
)

SOURCES_ion += ion/src/shared/collect_registers.cpp

_ion_simulator_files := 1

# RC file dependencies
$(call all_objects_for,$(PATH_ion)/src/simulator/windows/resources.rc): WRFLAGS += -I $(OUTPUT_DIRECTORY)
$(call all_objects_for,$(PATH_ion)/src/simulator/windows/resources.rc): $(addprefix $(OUTPUT_DIRECTORY)/,logo.ico)

$(addprefix $(OUTPUT_DIRECTORY)/,logo.ico): ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	magick convert -background "#FFB734" -resize 256x256 MSVG:$< $@

# Linker flags
LDFLAGS_ion += -lgdiplus -lcomdlg32

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

$(addprefix $(OUTPUT_DIRECTORY)/$(PATH_ion)/src/simulator/windows/,resources_gen.rc images.h): $(_ion_simulator_assets) | $$(@D)/.
	$(call rule_label,RESGEN)
	$(PYTHON) $(PATH_ion)/src/simulator/windows/resgen.py $^ -o $@

$(call all_objects_for,$(PATH_ion)/src/simulator/windows/platform_images.cpp): $(OUTPUT_DIRECTORY)/$(PATH_ion)/src/simulator/windows/images.h
$(call all_objects_for,$(PATH_ion)/src/simulator/windows/resources.rc): $(OUTPUT_DIRECTORY)/$(PATH_ion)/src/simulator/windows/resources_gen.rc
