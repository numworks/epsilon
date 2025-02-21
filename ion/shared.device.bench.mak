_sources_ion_bench := $(addprefix device/epsilon-core/device/bench/command/, \
  adc.cpp \
  backlight.cpp \
  charge.cpp \
  check_backup.cpp:-n0110 \
  command.cpp \
  crc.cpp \
  display.cpp \
  exit.cpp \
  external_flash_id.cpp \
  fill_backup.cpp:-n0110 \
  jump.cpp \
  keyboard.cpp \
  lcd_data.cpp \
  lcd_pins.cpp \
  lcd_timing.cpp \
  led.cpp \
  mcu_serial.cpp \
  ping.cpp \
  print.cpp \
  rtc_off.cpp:-n0110 \
  screen_id.cpp \
  sleep.cpp \
  standby.cpp \
  stop.cpp \
  time.cpp:-n0110 \
  usb_plugged.cpp \
  vblank.cpp \
) \
$(addprefix device/epsilon-core/device/bench/, \
  boot/isr.c:+ram \
  boot/rt0.cpp:+ram \
  command_handler.cpp \
  command_list.cpp \
  main.cpp \
) \
$(addprefix device/epsilon-core/device/bench/drivers/, \
  board.cpp \
  board_$(PLATFORM).cpp \
  console.cpp \
) \
$(addprefix device/epsilon-core/device/shared-core/drivers/, \
  backlight.cpp \
  backlight_advanced.cpp \
  backup_ram_$(PLATFORM).cpp \
  backup_ram.cpp \
  battery_$(_ion_mcu_suffix).cpp \
  battery_charge.cpp \
  board_frequency_$(_ion_mcu_suffix).cpp \
  board_power_supply_$(_ion_mcu_suffix).cpp \
  board_privileged.cpp \
  board_privileged_$(_ion_mcu_suffix).cpp \
  board_unprotected.cpp \
  cache.cpp \
  crc32.cpp \
  display.cpp \
  display_shortcut.cpp \
  display_vblank.cpp \
	external_flash.cpp \
  external_flash_qspi_$(_ion_mcu_suffix).cpp \
  internal_flash.cpp \
  internal_flash_$(_ion_mcu_suffix).cpp \
  internal_flash_otp.cpp \
  keyboard.cpp \
  keyboard_$(_ion_mcu_suffix).cpp \
  keyboard_init.cpp \
  keyboard_pins_$(_ion_mcu_suffix).cpp \
  led.cpp \
  power_$(_ion_mcu_suffix).cpp \
  power_standby_$(_ion_mcu_suffix).cpp \
  reset.cpp \
  rtc.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  usb_gpio_$(PLATFORM).cpp \
  wakeup.cpp \
) \
device/shared/boot/rt0.cpp \
$(addprefix device/shared/drivers/, \
  base64.cpp \
  serial_number.cpp \
  trampoline.cpp \
) \
device/shared/post_and_hardware_tests.cpp \
$(addprefix shared/, \
  console_line.cpp \
  display_context.cpp \
	exam_mode.cpp \
) \

_ldflags_ion_bench := \
  -Wl,-T,$(PATH_ion)/src/device/epsilon-core/device/bench/ram.ld

_lddeps_ion_bench := \
  $(PATH_ion)/src/device/epsilon-core/device/bench/ram.ld
