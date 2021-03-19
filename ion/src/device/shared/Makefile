include ion/src/device/shared/usb/Makefile

ion_device_shared_bootloader_kernel_src += $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/backlight.cpp \
  drivers/base64.cpp \
  drivers/display.cpp \
  drivers/external_flash_privileged.cpp \
  drivers/external_flash_unprivileged.cpp \
  drivers/flash_privileged.cpp \
  drivers/flash_unprivileged.cpp \
  drivers/internal_flash_privileged.cpp \
  drivers/internal_flash_unprivileged.cpp \
  drivers/reset.cpp \
  drivers/serial_number.cpp \
  drivers/timing.cpp \
  drivers/usb_privileged.cpp \
  drivers/usb_unprivileged.cpp \
  drivers/usb.cpp \
  warning_display.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/usb.cpp \
  drivers/usb_unprivileged.cpp \
  drivers/external_flash_unprivileged.cpp \
  drivers/flash_unprivileged.cpp \
  drivers/internal_flash_unprivileged.cpp \
)
