# Shared
include ion/src/device/shared/usb/Makefile

ion_device_shared_bootloader_flasher_kernel_src = $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/backlight.cpp \
  drivers/base64.cpp \
  drivers/board_privileged.cpp \
  drivers/display.cpp \
  drivers/internal_flash.cpp \
  drivers/internal_flash_otp.cpp \
  drivers/reset.cpp \
  drivers/serial_number.cpp \
  drivers/timing.cpp \
  drivers/usb.cpp \
  drivers/usb_privileged.cpp \
  drivers/usb_unprivileged.cpp \
)

ion_device_shared_bootloader_kernel_src = $(addprefix ion/src/device/shared/drivers/, \
  battery.cpp \
  keyboard.cpp \
  random.cpp \
)
ion_device_shared_bootloader_kernel_src += $(ion_device_shared_bootloader_flasher_kernel_src)

# Target specific

ion_device_bootloader_src += ion/src/device/shared/post_and_hardware_tests.cpp ion/src/device/shared/drivers/display_image.cpp ion/src/device/shared/drivers/backlight_basic.cpp $(ion_device_shared_bootloader_kernel_src) $(ion_device_dfu_src)

ion_device_flasher_src += ion/src/device/shared/drivers/backlight_basic.cpp $(ion_device_shared_bootloader_flasher_kernel_src) $(ion_device_dfu_src)

ion_device_kernel_src += $(addprefix ion/src/device/shared/drivers/, \
  board_unprivileged.cpp \
  trampoline.cpp \
)
ion_device_kernel_src += $(ion_device_shared_bootloader_kernel_src) $(ion_device_dfu_src)

ion_device_userland_src += $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/board_unprivileged.cpp \
  drivers/trampoline.cpp \
  drivers/usb.cpp \
  drivers/usb_unprivileged.cpp \
  post_and_hardware_tests.cpp \
)
ion_device_userland_src += $(ion_device_dfu_src)
