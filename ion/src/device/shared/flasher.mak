include ion/src/device/shared/usb/Makefile

ion_device_flasher_src += $(addprefix ion/src/device/shared/boot/, \
  isr.c \
  rt0.cpp \
  rt0_n0110.cpp \
)

ion_device_flasher_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  backlight_basic.cpp \
  base64.cpp \
  board_privileged.cpp \
  board_privileged_n0110.cpp \
  board_unprotected.cpp \
  display.cpp \
  external_flash.cpp \
  external_flash_command.cpp \
  internal_flash.cpp \
  internal_flash_otp.cpp \
  reset.cpp \
  serial_number.cpp \
  timing.cpp \
  usb.cpp \
  usb_n0110.cpp \
  usb_privileged.cpp \
  usb_unprivileged.cpp \
)

ion_device_flasher_src += $(ion_device_dfu_src)
