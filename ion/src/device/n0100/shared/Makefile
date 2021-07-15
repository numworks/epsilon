include ion/src/device/shared/usb/Makefile

# Kernel/Userland

ion_device_kernel_userland_src += $(addprefix ion/src/device/shared/, \
  drivers/board_unprivileged.cpp \
)

ion_device_kernel_userland_src += $(addprefix ion/src/device/n0100/shared/drivers/, \
  flash_unprivileged.cpp \
)

# Kernel

ion_device_kernel_src += $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/backlight.cpp \
  drivers/base64.cpp \
  drivers/battery.cpp \
  drivers/board_privileged.cpp \
  drivers/display.cpp \
  drivers/display_image.cpp \
  drivers/keyboard.cpp \
  drivers/internal_flash.cpp \
  drivers/random.cpp \
  drivers/reset.cpp \
  drivers/serial_number.cpp \
  drivers/timing.cpp \
  drivers/usb.cpp \
  drivers/usb_privileged.cpp \
  drivers/usb_unprivileged.cpp \
)

ion_device_kernel_src += $(ion_device_kernel_userland_src)

$(eval $(call requires_compressed_png,ion/src/device/shared/drivers/display_image.cpp,ion/src/device/shared/drivers/logo_image,ion_device_kernel_src))

# Userland

ion_device_userland_src += $(ion_device_kernel_userland_src)
