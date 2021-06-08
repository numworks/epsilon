include ion/src/device/shared/usb/Makefile

ion_device_bootloader_src += $(addprefix ion/src/device/shared/, \
  post_and_hardware_tests.cpp \
)

ion_device_shared_bootloader_kernel_src += $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/backlight.cpp \
  drivers/battery.cpp \
  drivers/base64.cpp \
  drivers/display.cpp \
  drivers/keyboard.cpp \
  drivers/internal_flash.cpp \
  drivers/random.cpp \
  drivers/reset.cpp \
  drivers/serial_number.cpp \
  drivers/timing.cpp \
  drivers/usb_privileged.cpp \
  drivers/usb_unprivileged.cpp \
  drivers/usb.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/usb.cpp \
  drivers/usb_unprivileged.cpp \
  post_and_hardware_tests.cpp \
)
