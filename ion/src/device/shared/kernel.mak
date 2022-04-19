ion_device_kernel_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_kernel_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  backlight_advanced.cpp \
  base64.cpp \
  battery_stm32f.cpp:-n0120 \
  battery_stm32h.cpp:+n0120 \
  battery_charge.cpp \
  board_frequency_stm32f.cpp:-n0120 \
  board_frequency_stm32h.cpp:+n0120 \
  board_power_supply_n0120.cpp:+n0120 \
  board_privileged.cpp \
  board_privileged_dual_slots.cpp:-n0100 \
  board_privileged_n0110.cpp:+n0110 \
  board_privileged_n0120.cpp:+n0120 \
  board_privileged_standalone.cpp:+n0100 \
  board_unprivileged.cpp \
  board_unprivileged_standalone.cpp:+n0100 \
  board_unprivileged_dual_slots.cpp:-n0100 \
  cache.cpp:-n0100 \
  crc32.cpp \
  display.cpp \
  display_image.cpp:+n0100 \
  flash.cpp:+n0100 \
  flash_privileged.cpp:-n0100 \
  flash_unprivileged.cpp:-n0100 \
  internal_flash.cpp \
  internal_flash_otp_dual_slots.cpp:-n0100 \
  keyboard.cpp \
  led.cpp \
  option_bytes.cpp \
  option_bytes_n0110.cpp:+n0110 \
  option_bytes_n0120.cpp:+n0120 \
  power_stm32f.cpp:-n0120 \
  power_stm32h.cpp:+n0120 \
  power_standby_n0110.cpp:+n0110 \
  power_standby_n0120.cpp:+n0120 \
  random.cpp \
  reset.cpp \
  serial_number.cpp \
  swd.cpp \
  timing.cpp \
  trampoline.cpp:-n0100 \
  usb.cpp \
  usb_exit_condition.cpp \
  usb_gpio_n0100.cpp:+n0100 \
  usb_gpio_n0110.cpp:+n0110 \
  usb_gpio_n0120.cpp:+n0120 \
  usb_unprivileged.cpp \
  wakeup.cpp \
)

$(eval $(call requires_compressed_png,ion/src/device/shared/drivers/display_image.cpp,ion/src/device/shared/drivers/logo_image,ion_device_kernel_src))
