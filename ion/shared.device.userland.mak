_sources_ion_userland_svc := $(addprefix device/userland/drivers/, \
  authentication.cpp \
  backlight.cpp \
  battery.cpp \
  board.cpp \
  circuit_breaker.cpp \
  compilation_flags.cpp \
  crc32.cpp \
  device_name.cpp \
  display.cpp \
  events.cpp \
  exam_bytes.cpp \
  fcc_id.cpp \
  flash_write_with_interruptions.cpp \
  init.cpp \
  keyboard.cpp \
  led.cpp \
  pcb_version.cpp \
  persisting_bytes.cpp \
  power.cpp \
  random.cpp \
  reset_svcall.cpp \
  serial_number_svcall.cpp \
  timing.cpp \
  usb.cpp \
  usb_svcall.cpp \
)

_sources_ion_userland := \
$(addprefix device/userland/boot/, \
  isr.cpp \
  rt0.cpp \
) \
$(addprefix device/userland/drivers/, \
  board_dual_slots.cpp \
  board_ram_dfu_xip.cpp \
  clipboard.cpp \
  console_dummy.cpp:-consoledisplay \
  external_apps.cpp:+allow3rdparty \
  reset.cpp \
  serial_number.cpp \
  stack.cpp \
  userland_header.cpp \
) \
$(addprefix device/shared/, \
  boot/rt0.cpp \
  post_and_hardware_tests.cpp \
) \
$(addprefix device/shared/drivers/, \
  assert.cpp \
  board_unprivileged.cpp \
  console_display.cpp:+consoledisplay \
  flash_information.cpp \
  trampoline.cpp \
  usb_unprivileged.cpp \
) \
$(addprefix shared/, \
  dummy/external_apps.cpp:-allow3rdparty \
  collect_registers.cpp \
  console_line.cpp \
  display_context.cpp \
  exam_mode.cpp \
  stack_position.cpp \
  storage/file_system.cpp \
  storage/record_name_verifier.cpp \
  storage/record.cpp \
) \
  $(_sources_ion_userland_svc)

ifneq ($(ION_LOG_EVENTS_NAME),0)
_sources_ion_userland += shared/layout_events/$(ION_layout_variant)/layout_events.cpp
endif

$(call all_objects_for,$(addprefix $(PATH_ion)/src/,$(_sources_ion_userland_svc))): SFLAGS += -fno-lto

_ldflags_ion_userland := \
  -Wl,-T,$(PATH_ion)/src/device/userland/flash/userland_A.ld:+A \
  -Wl,-T,$(PATH_ion)/src/device/userland/flash/userland_B.ld:+B \
  -L$(PATH_ion)/src/device/userland/flash

_lddeps_ion_userland := \
  $(PATH_ion)/src/device/userland/flash/userland_A.ld:+A \
  $(PATH_ion)/src/device/userland/flash/userland_B.ld:+B
