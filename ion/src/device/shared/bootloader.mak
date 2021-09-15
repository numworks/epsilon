include ion/src/device/shared/usb/Makefile

ion_device_bootloader_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_bootloader_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  backlight_basic.cpp \
  base64.cpp \
  battery.cpp \
  board_privileged.cpp \
  board_privileged_n0110.cpp \
  display.cpp \
  display_image.cpp \
  display_shortcut.cpp \
  display_vblank.cpp \
  external_flash.cpp \
  external_flash_command.cpp \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
  internal_flash.cpp \
  internal_flash_otp.cpp \
  keyboard.cpp \
  keyboard_init.cpp \
  power_configuration.cpp \
  power_suspend.cpp \
  random.cpp \
  reset.cpp \
  serial_number.cpp \
  timing.cpp \
  usb.cpp \
  usb_n0110.cpp \
  usb_privileged.cpp \
  usb_unprivileged.cpp \
)

ion_device_bootloader_src += ion/src/device/shared/post_and_hardware_tests
ion_device_bootloader_src += $(ion_device_dfu_src)
