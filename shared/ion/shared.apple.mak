# Packaged simulator assets

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

_ion_simulator_icons := $(patsubst %,$(_ion_simulator_iconset)/icon_%.png,$(_ion_simulator_icons_sizes))

$(_ion_simulator_icons): $(_ion_simulator_iconset)/icon_%.png: $(PATH_ion)/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
ifeq ($(_ion_simulator_icons_use_mask),1)
	convert -background "#$(_ion_icon_background)" MSVG:$< -gravity center -scale 80% -extent 1024x1024 MSVG:$(PATH_ion)/src/simulator/assets/icon_mask.svg -alpha Off -compose CopyOpacity -composite -resize $* $@
else
	convert -background "#$(_ion_icon_background)" -resize $* MSVG:$< $@
endif

# Localization resources
_ion_simulator_lproj_dirs := $(wildcard $(PATH_ion)/src/simulator/shared/apple/resources/$(ION_variant)/*.lproj)

$(eval $(foreach d,$(_ion_simulator_lproj_dirs),\
$(call rule_for_simulator_resource,COPY,$(notdir $d),$d,\
  cp -r $$^ $$@ \
)))
