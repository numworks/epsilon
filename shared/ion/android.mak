SOURCES_ion += $(addprefix  $(PATH_ion)/src/simulator/android/src/cpp/, \
  haptics_enabled.cpp \
  platform_images.cpp \
  platform_language.cpp \
)

SOURCES_ion += $(addprefix  $(PATH_ion)/src/simulator/shared/, \
  dummy/journal.cpp \
  dummy/keyboard_callback.cpp \
  dummy/window_callback.cpp \
  circuit_breaker.cpp \
  clipboard_helper_sdl.cpp \
  haptics.cpp \
)

SOURCES_ion += $(addprefix $(PATH_ion)/src/shared/, \
  collect_registers.cpp \
)

$(call all_objects_for,$(PATH_ion)/src/simulator/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

LDFLAGS_ion += -ljnigraphics -llog

_ion_simulator_files := 0

# Simulator resources

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


# Android resources
# Some android resources needs to be filtered through ImageMagick. Others are
# simply copied over.

$(OUTPUT_DIRECTORY)/app/res/mipmap/ic_launcher.png: ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background "#FFB734" MSVG:$< $@

$(OUTPUT_DIRECTORY)/app/res/mipmap-v26/ic_launcher_foreground.png: ion/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
	$(Q) convert -background none MSVG:$< -resize 512x512 -gravity center -background none -extent 1024x1024 $@

$(OUTPUT_DIRECTORY)/app/res/%.xml: ion/src/simulator/android/src/res/%.xml | $$(@D)/.
	$(call rule_label,COPY)
	$(Q) cp $< $@
