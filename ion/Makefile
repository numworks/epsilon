# TODO
# include ion/build/$(PLATFORM).mak

_sources_ion_all := $(addprefix src/, \
  shared/collect_registers.cpp \
  shared/console_line.cpp \
  shared/display_context.cpp \
  shared/dummy/authentication.cpp \
  shared/dummy/backlight.cpp \
  shared/dummy/battery.cpp \
  shared/dummy/display.cpp \
  shared/dummy/external_apps.cpp \
  shared/dummy/fcc_id.cpp \
  shared/dummy/led.cpp \
  shared/dummy/platform_info.cpp \
  shared/dummy/post_and_hardware_tests.cpp \
  shared/dummy/power.cpp \
  shared/dummy/reset.cpp \
  shared/dummy/stack.cpp \
  shared/events.cpp \
  shared/dummy/usb.cpp \
  shared/events_modifier.cpp \
  shared/exam_mode.cpp \
  shared/keyboard_queue.cpp \
  shared/keyboard.cpp \
  shared/layout_events.cpp \
  shared/stack_position.cpp \
  shared/storage/file_system.cpp \
  shared/storage/record.cpp \
  shared/storage/record_name_verifier.cpp \
  simulator/macos/platform_files.mm \
  simulator/shared/actions.cpp \
  simulator/shared/apple/platform_images.mm \
  simulator/shared/apple/platform_language.mm \
  simulator/shared/circuit_breaker.cpp \
  simulator/shared/clipboard.cpp \
  simulator/shared/clipboard_helper.cpp \
  simulator/shared/clipboard_helper_sdl.cpp \
  simulator/shared/compilation_flags.cpp \
  simulator/shared/console.cpp \
  simulator/shared/crc32.cpp \
  simulator/shared/device_name.cpp \
  simulator/shared/display.cpp \
  simulator/shared/dummy/haptics_enabled.cpp \
  simulator/shared/dummy/keyboard_callback.cpp \
  simulator/shared/dummy/window_callback.cpp \
  simulator/shared/events.cpp \
  simulator/shared/events_platform.cpp \
  simulator/shared/exam_bytes.cpp \
  simulator/shared/framebuffer.cpp \
  simulator/shared/haptics.cpp \
  simulator/shared/init.cpp \
  simulator/shared/journal.cpp \
  simulator/shared/keyboard.cpp \
  simulator/shared/main.cpp \
  simulator/shared/platform_files.cpp \
  simulator/shared/random.cpp \
  simulator/shared/screenshot.cpp \
  simulator/shared/state_file.cpp \
  simulator/shared/timing.cpp \
  simulator/shared/unix/platform_files.cpp \
  simulator/shared/window.cpp \
)

$(eval $(call create_module,ion,1, \
	$(_sources_ion_all) \
))

SFLAGS_ion += \
  -DION_EVENTS_JOURNAL

PRIVATE_SFLAGS_ion += \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"

# Simulator files - begin
# TODO ION_SIMULATOR_FILES should be a flavor instead of a flag

# TODO macos only
_ion_simulator_window_setup := $(PATH_ion)/src/simulator/macos/window.mm

# TODO Specialize per platform
_ion_simulator_files := 1
ifeq ($(_ion_simulator_files),0)
_ion_simulator_window_setup ?= $(PATH_ion)/src/simulator/shared/dummy/window_position.cpp
else
_ion_simulator_window_setup ?= $(PATH_ion)/src/simulator/shared/window_position_cached.cpp
SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/shared/, \
  actions.cpp \
  state_file.cpp \
  screenshot.cpp \
  platform_files.cpp \
)
# TODO Move to eadk module?
SOURCES_ion += eadk/src/simulator.cpp
SFLAGS_ion += -DION_SIMULATOR_FILES=1
# Export symbols so that dlopen-ing NWB files can use eadk_external_data and eadk_external_data_size
LDFLAGS_ion += $(LD_EXPORT_SYMBOLS_FLAG)
endif
SOURCES_ion += $(_ion_simulator_window_setup)

# Simulator files - end

# Simulator layout
_sources_ion_simulator_layout := $(PATH_ion)/src/simulator/shared/layout.cpp
$(OUTPUT_DIRECTORY)/$(_sources_ion_simulator_layout): $(PATH_ion)/src/simulator/shared/layout.json $(PATH_ion)/src/simulator/shared/layout.py | $$(@D)/.
	$(call rule_label,LAYOUT)
	$(PYTHON) $(filter %.py,$^) -i $(filter %.json,$^) -o $@
SOURCES_ion += $(_sources_ion_simulator_layout)

# Packaged simulator assets

_ion_simulator_iconset := $(OUTPUT_DIRECTORY)/app/assets/app.iconset
_ion_simulator_icons_sizes := 16x16 32x32 64x64 128x128 256x256 512x512 1024x1024
_ion_simulator_icons := $(patsubst %,$(_ion_simulator_iconset)/icon_%.png,$(_ion_simulator_icons_sizes))
_ion_simulator_background := $(PATH_ion)/src/simulator/assets/background-with-shadow.webp
_ion_simulator_backgrounds_generated := $(addprefix $(OUTPUT_DIRECTORY)/app/assets/,background.jpg background-no-shadow.webp)
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

# FIXME Sizes and offsets should be parameterized
$(_ion_simulator_backgrounds_generated): $(_ion_simulator_background) | $$(@D)/.
	$(call rule_label,CONVERT)
	$(QUIET) convert -crop 1005x1975+93+13 -resize 1160x2220 $< $@

$(eval $(call rule_for_simulator_resource, \
  ICNUTIL,app.icns,$(_ion_simulator_icons), \
  iconutil --convert icns --output $$@ $$(dir $$<) \
))

$(_ion_simulator_icons): $(_ion_simulator_iconset)/icon_%.png: $(PATH_ion)/src/simulator/assets/logo.svg | $$(@D)/.
	$(call rule_label,CONVERT)
ifeq ($(SIMULATOR_ICON_USE_MASK),1)
	$(QUIET) convert -background "#FFB734" MSVG:$< -gravity center -scale 80% -extent 1024x1024 MSVG:$(PATH_ion)/src/simulator/assets/icon_mask.svg -alpha Off -compose CopyOpacity -composite -resize $* $@
else
	$(QUIET) convert -background "#FFB734" -resize $* MSVG:$< $@
endif
