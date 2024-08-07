_sources_ion_kernel := \
$(addprefix shared/, \
  console_line.cpp:+kernelassert \
  display_context.cpp:+kernelassert \
  events.cpp \
  events_modifier.cpp \
  keyboard.cpp \
  keyboard_queue.cpp \
  layout_events/$(ION_layout_variant)/layout_events.cpp \
) \
$(addprefix device/shared/boot/, \
  rt0.cpp \
) \
$(addprefix device/shared/drivers/, \
  assert.cpp:+kernelassert \
  base64.cpp \
  board_unprivileged.cpp \
  console_display.cpp:+kernelassert \
  flash_information.cpp\
  serial_number.cpp \
  trampoline.cpp \
  usb_unprivileged.cpp \
) \
$(addprefix device/epsilon-core/device/shared-core/drivers/, \
  backlight.cpp \
  backlight_advanced.cpp \
  battery_$(_ion_mcu_suffix).cpp \
  battery_charge.cpp \
  board_frequency_$(_ion_mcu_suffix).cpp \
  board_privileged.cpp \
  board_privileged_$(_ion_mcu_suffix).cpp \
  cache.cpp \
  crc32.cpp \
  display.cpp \
  display_shortcut.cpp:+kernelassert \
  flash_write.cpp \
  internal_flash.cpp \
  internal_flash_otp.cpp \
  keyboard.cpp \
  keyboard_$(_ion_mcu_suffix).cpp \
  led.cpp \
  option_bytes.cpp \
  option_bytes_$(_ion_mcu_suffix).cpp \
  power_$(_ion_mcu_suffix).cpp \
  power_standby_$(_ion_mcu_suffix).cpp \
  random.cpp \
  random_$(_ion_mcu_suffix).cpp \
  reset.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  usb_gpio_$(PLATFORM).cpp \
  wakeup.cpp \
) \
$(addprefix device/epsilon-core/device/kernel/, \
  main.cpp \
  warning_display.cpp:+allow3rdparty \
) \
$(addprefix device/epsilon-core/device/kernel/boot/, \
  isr_$(_ion_mcu_suffix).c \
  rt0.cpp \
) \
$(addprefix device/epsilon-core/device/kernel/drivers/, \
  authentication.cpp:+allow3rdparty \
  authentication_$(_ion_mcu_suffix).cpp:+allow3rdparty \
  authentication_strict.cpp:-allow3rdparty \
  battery.cpp \
  board.cpp \
  board_$(_ion_mcu_suffix).cpp \
  board_third_party.cpp:+allow3rdparty \
  board_third_party_strict.cpp:-allow3rdparty \
  circuit_breaker.cpp \
  compilation_flags.cpp \
  cortex_control.s \
  crc32.cpp \
  display.cpp \
  events.cpp \
  external_flash.cpp \
  fcc_id.cpp \
  flash_authorizations.cpp \
  flash_write_with_interruptions.cpp \
  init.cpp \
  kernel_header.cpp \
  keyboard.cpp \
  keyboard_$(_ion_mcu_suffix).cpp \
  led.cpp \
  led_update.cpp \
  power.cpp \
  power_$(_ion_mcu_suffix).cpp \
  serial_number.cpp \
  svcall_handler.cpp \
  svcall_handler_as.s \
  timing.cpp \
  usb.cpp \
  wakeup.cpp \
  wakeup_$(_ion_mcu_suffix).cpp \
) \

ifeq ($(PLATFORM),n0120)
_sources_ion_kernel += $(addprefix device/epsilon-core/device/shared-core/drivers/, \
  board_power_supply_stm32h.cpp \
  keyboard_pins_stm32h.cpp \
)
endif

_ldflags_ion_kernel := \
  -Wl,-T,$(PATH_ion)/src/device/epsilon-core/device/kernel/flash/kernel_A.ld:+A \
  -Wl,-T,$(PATH_ion)/src/device/epsilon-core/device/kernel/flash/kernel_B.ld:+B \
  -L$(PATH_ion)/src/device/epsilon-core/device/kernel/flash

_lddeps_ion_kernel := \
  $(PATH_ion)/src/device/epsilon-core/device/kernel/flash/kernel_A.ld:+A \
  $(PATH_ion)/src/device/epsilon-core/device/kernel/flash/kernel_B.ld:+B
