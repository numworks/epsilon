include ion/src/device/n0100/shared/usb/Makefile

# Kernel/Userland

ion_device_kernel_userland_src += $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/base64.cpp \
  drivers/board_unprivileged.cpp \
  drivers/usb_unprivileged.cpp \
  drivers/serial_number.cpp \
)

ion_device_kernel_userland_src += $(addprefix ion/src/device/n0100/shared/drivers/, \
  board_unprivileged.cpp \
  flash.cpp \
)

# Kernel

ion_device_kernel_src += $(addprefix ion/src/device/shared/, \
  drivers/backlight.cpp \
  drivers/backlight_advanced.cpp \
  drivers/battery.cpp \
  drivers/battery_charge.cpp \
  drivers/board_frequency.cpp \
  drivers/board_privileged.cpp \
  drivers/crc32.cpp \
  drivers/display.cpp \
  drivers/display_image.cpp \
  drivers/internal_flash.cpp \
  drivers/keyboard.cpp \
  drivers/led.cpp \
  drivers/power.cpp \
  drivers/random.cpp \
  drivers/reset.cpp \
  drivers/timing.cpp \
  drivers/usb.cpp \
  drivers/usb_privileged.cpp \
  drivers/wakeup.cpp \
)

ion_device_kernel_src += $(ion_device_kernel_userland_src)

$(eval $(call requires_compressed_png,ion/src/device/shared/drivers/display_image.cpp,ion/src/device/shared/drivers/logo_image,ion_device_kernel_src))

# Userland

ion_device_userland_src += $(addprefix ion/src/device/shared/, \
  post_and_hardware_tests.cpp \
)

ion_device_userland_src += $(ion_device_kernel_userland_src) $(ion_device_dfu_bin_src)
