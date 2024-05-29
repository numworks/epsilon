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
  simulator/macos/window.mm \
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
  -DION_SIMULATOR_FILES=1 \
  -DION_EVENTS_JOURNAL

PRIVATE_SFLAGS_ion += -DEPSILON_VERSION=\"$(APP_VERSION)\" -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"
