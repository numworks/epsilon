include ion/src/device/shared/usb/Makefile

ion_device_bootloader_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_bootloader_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  backlight_basic.cpp \
  base64.cpp \
  battery_stm32f.cpp:-n0120 \
  battery_stm32h.cpp:+n0120 \
  board_frequency_stm32f.cpp:-n0120 \
  board_frequency_stm32h.cpp:+n0120 \
  board_peripherals_clocks_n0110.cpp:+n0110 \
  board_peripherals_clocks_n0120.cpp:+n0120 \
  board_privileged.cpp \
  board_privileged_n0110.cpp:+n0110 \
  board_privileged_n0120.cpp:+n0120 \
  display.cpp \
  display_image.cpp \
  display_shortcut.cpp \
  display_vblank.cpp \
  external_flash.cpp \
  external_flash_qspi_n0110.cpp:+n0110 \
  external_flash_qspi_n0120.cpp:+n0120 \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
  internal_flash.cpp \
  internal_flash_otp_n0110.cpp:+n0110 \
  internal_flash_otp_n0120.cpp:+n0120 \
  keyboard.cpp \
  keyboard_init.cpp \
  power_suspend.cpp \
  power_standby_n0110.cpp:+n0110 \
  power_standby_n0120.cpp:+n0120 \
  random.cpp \
  reset.cpp \
  swd.cpp \
  serial_number.cpp \
  timing.cpp \
  usb.cpp \
  usb_gpio_n0110.cpp:+n0110 \
  usb_gpio_n0120.cpp:+n0120 \
)

ion_device_bootloader_src += ion/src/device/shared/post_and_hardware_tests
ion_device_bootloader_src += $(ion_device_dfu_src)
