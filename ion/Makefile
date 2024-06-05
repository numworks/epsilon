$(call create_module,ion,1,$(addprefix src/, \
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
  simulator/shared/clipboard.cpp \
  simulator/shared/clipboard_helper.cpp \
  simulator/shared/compilation_flags.cpp \
  simulator/shared/console.cpp \
  simulator/shared/crc32.cpp \
  simulator/shared/device_name.cpp \
  simulator/shared/display.cpp \
  simulator/shared/events.cpp \
  simulator/shared/events_platform.cpp \
  simulator/shared/exam_bytes.cpp \
  simulator/shared/framebuffer.cpp \
  simulator/shared/init.cpp \
  simulator/shared/keyboard.cpp \
  simulator/shared/main.cpp \
  simulator/shared/random.cpp \
  simulator/shared/timing.cpp \
  simulator/shared/window.cpp \
))

SFLAGS_ion += \
  -DION_EVENTS_JOURNAL

PRIVATE_SFLAGS_ion += \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"

# Simulator backgrounds - begin

_ion_simulator_background := $(PATH_ion)/src/simulator/assets/background-with-shadow.webp
_ion_simulator_backgrounds_generated := $(addprefix $(OUTPUT_DIRECTORY)/app/assets/,background.jpg background-no-shadow.webp)

# FIXME Sizes and offsets should be parameterized
$(_ion_simulator_backgrounds_generated): $(_ion_simulator_background) | $$(@D)/.
	$(call rule_label,CONVERT)
	$(QUIET) convert -crop 1005x1975+93+13 -resize 1160x2220 $< $@

# Simulator backgrounds - end

include $(PATH_ion)/$(PLATFORM).mak

# Simulator files - begin
# TODO ION_SIMULATOR_FILES should be a flavor instead of a flag

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
	$(QUIET) $(PYTHON) $(filter %.py,$^) -i $(filter %.json,$^) -o $@
SOURCES_ion += $(_sources_ion_simulator_layout)

