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

simulator_app_deps = $(addprefix ion/src/simulator/assets/,epsilon/background-with-shadow.webp horizontal_arrow.png vertical_arrow.png round.png small_squircle.png large_squircle.png)
$(addprefix $(OUTPUT_DIRECTORY)/,$(addprefix ion/src/simulator/assets/,background.jpg background-no-shadow.webp)): ion/src/simulator/assets/epsilon/background-with-shadow.webp
	$(call rule_label,CONVERT)
	$(Q) mkdir -p $(dir $@)
	$(Q) convert -crop 1005x1975+93+13 -resize 1160x2220 $< $@
simulator_app_deps += $(addprefix $(OUTPUT_DIRECTORY)/,$(addprefix ion/src/simulator/assets/,background.jpg background-no-shadow.webp))

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
