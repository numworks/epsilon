ion_device_bench_src += $(addprefix ion/src/device/shared/boot/, \
  isr.c \
  rt0.cpp \
  rt0_n0110.cpp \
)

ion_device_bench_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  backlight_advanced.cpp \
  base64.cpp \
  battery_stm32f.cpp:-n0120 \
  battery_stm32h.cpp:+n0120 \
  battery_charge.cpp \
  board_frequency_stm32f.cpp:-n0120 \
  board_frequency_stm32h.cpp:+n0120 \
  board_peripherals_clocks_n0110.cpp:+n0110 \
  board_peripherals_clocks_n0120.cpp:+n0120 \
  board_privileged.cpp \
  board_privileged_dual_slots.cpp:-n0100 \
  board_privileged_n0110.cpp:+n0110 \
  board_privileged_n0120.cpp:+n0120 \
  board_privileged_standalone.cpp:+n0100 \
  board_unprotected.cpp \
  cache.cpp \
  crc32.cpp \
  display.cpp \
  display_shortcut.cpp \
  display_vblank.cpp \
  external_flash.cpp \
  external_flash_qspi_n0110.cpp:+n0110 \
  external_flash_qspi_n0120.cpp:+n0120 \
  internal_flash.cpp \
  internal_flash_otp_dual_slots.cpp:-n0100 \
  keyboard.cpp \
  keyboard_init.cpp \
  led.cpp \
  power_stm32f.cpp:-n0120 \
  power_stm32h.cpp:+n0120 \
  power_standby_n0110.cpp:+n0110 \
  power_standby_n0120.cpp:+n0120 \
  power_suspend.cpp \
  reset.cpp \
  serial_number.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  usb_gpio_n0100.cpp:+n0100 \
  usb_gpio_n0110.cpp:+n0110 \
  usb_gpio_n0120.cpp:+n0120 \
  wakeup.cpp \
)

ion_device_bench_src += ion/src/device/shared/post_and_hardware_tests
