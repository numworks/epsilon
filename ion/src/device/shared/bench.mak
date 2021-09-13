ion_device_bench_src += $(addprefix ion/src/device/shared/boot/, \
  isr.c \
  rt0.cpp \
  rt0_n0110.cpp \
)

ion_device_bench_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  backlight_advanced.cpp \
  base64.cpp \
  battery.cpp \
  battery_charge.cpp \
  board_frequency.cpp \
  board_privileged.cpp \
  board_privileged_n0110.cpp \
  board_unprotected.cpp \
  cache.cpp \
  crc32.cpp \
  display.cpp \
  display_shortcut.cpp \
  display_vblank.cpp \
  external_flash.cpp \
  external_flash_command.cpp \
  internal_flash_otp.cpp \
  keyboard.cpp \
  keyboard_init.cpp \
  led.cpp \
  power.cpp \
  power_configuration.cpp \
  power_suspend.cpp \
  reset.cpp \
  serial_number.cpp \
  timing.cpp \
  usb_n0110.cpp \
  usb_privileged.cpp \
  wakeup.cpp \
)

ion_device_bench_src += ion/src/device/shared/post_and_hardware_tests
