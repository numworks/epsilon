SOURCES_ion += $(addprefix $(PATH_ion)/src/, \
$(addprefix shared/, \
  console_line.cpp \
  display_context.cpp \
  events.cpp \
  events_modifier.cpp \
  exam_mode.cpp \
  keyboard_queue.cpp \
  keyboard.cpp \
  layout_events.cpp \
  stack_position.cpp \
  storage/file_system.cpp \
  storage/record.cpp \
  storage/record_name_verifier.cpp \
) \
$(addprefix shared/dummy/, \
  authentication.cpp \
  backlight.cpp \
  battery.cpp \
  display_misc.cpp \
  external_apps.cpp \
  fcc_id.cpp \
  led.cpp \
  platform_info.cpp \
  post_and_hardware_tests.cpp \
  power.cpp \
  reset.cpp \
  stack.cpp \
  usb.cpp \
) \
$(addprefix simulator/shared/, \
  clipboard.cpp \
  clipboard_helper.cpp \
  compilation_flags.cpp \
  console.cpp \
  crc32.cpp \
  device_name.cpp \
  display.cpp \
  events.cpp \
  events_platform.cpp \
  exam_bytes.cpp \
  framebuffer.cpp \
  init.cpp \
  keyboard.cpp \
  main.cpp \
  random.cpp \
  timing.cpp \
  window.cpp \
) \
)

SFLAGS_ion += \
  -DION_EVENTS_JOURNAL

# Simulator backgrounds - begin

$(call assert_defined,ION_LAYOUT_VARIANT)

_ion_simulator_background := $(PATH_ion)/src/simulator/assets/$(ION_LAYOUT_VARIANT)/background-with-shadow.webp
_ion_simulator_backgrounds_generated := $(addprefix $(OUTPUT_DIRECTORY)/app/assets/,background.jpg background-no-shadow.webp)

_ion_simulator_background_crop_epsilon := 1005x1975+93+13
_ion_simulator_background_resize_epsilon := 1160x2220

# FIXME Sizes and offsets should be parameterized
$(_ion_simulator_backgrounds_generated): $(_ion_simulator_background) | $$(@D)/.
	$(call rule_label,CONVERT)
	convert \
		-crop $(_ion_simulator_background_crop_$(ION_LAYOUT_VARIANT)) \
		-resize $(_ion_simulator_background_resize_$(ION_LAYOUT_VARIANT)) \
		$< $@

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
$(OUTPUT_DIRECTORY)/$(_sources_ion_simulator_layout): $(PATH_ion)/src/simulator/shared/layout/$(ION_LAYOUT_VARIANT).json $(PATH_ion)/src/simulator/shared/layout.py | $$(@D)/.
	$(call rule_label,LAYOUT)
	$(PYTHON) $(filter %.py,$^) -i $(filter %.json,$^) -o $@
SOURCES_ion += $(_sources_ion_simulator_layout)
