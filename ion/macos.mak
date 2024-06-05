SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  macos/platform_files.mm \
  shared/apple/platform_images.mm \
  shared/apple/platform_language.mm \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/keyboard_callback.cpp \
  shared/dummy/window_callback.cpp \
  shared/unix/platform_files.cpp \
  shared/haptics.cpp \
  shared/circuit_breaker.cpp \
  shared/clipboard_helper_sdl.cpp \
  shared/journal.cpp \
)

# TODO collect_registers.cpp depends on arch, tweak haussmann
SOURCES_ion += $(PATH_ion)/src/shared/collect_registers.cpp

_ion_simulator_window_setup := $(PATH_ion)/src/simulator/macos/window.mm
_ion_simulator_files := 1

# Packaged simulator assets

_ion_simulator_iconset := $(OUTPUT_DIRECTORY)/app/assets/app.iconset
_ion_simulator_icons_sizes := 16x16 32x32 64x64 128x128 256x256 512x512 1024x1024
_ion_simulator_icons := $(patsubst %,$(_ion_simulator_iconset)/icon_%.png,$(_ion_simulator_icons_sizes))
# TODO macos only
_ion_simulator_icons_use_mask := 1

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

$(eval $(foreach a,$(_ion_simulator_assets),\
$(call rule_for_simulator_resource,COPY,$(notdir $a),$a,\
  cp $$^ $$@ \
)))

$(call rule_for_simulator_resource, \
  ICNUTIL,app.icns,$(_ion_simulator_icons), \
  iconutil --convert icns --output $$@ $$(dir $$<) \
)

$(_ion_simulator_icons): $(_ion_simulator_iconset)/icon_%.png: $(PATH_ion)/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
ifeq ($(SIMULATOR_ICON_USE_MASK),1)
	$(QUIET) convert -background "#FFB734" MSVG:$< -gravity center -scale 80% -extent 1024x1024 MSVG:$(PATH_ion)/src/simulator/assets/icon_mask.svg -alpha Off -compose CopyOpacity -composite -resize $* $@
else
	$(QUIET) convert -background "#FFB734" -resize $* MSVG:$< $@
endif
