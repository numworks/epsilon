ion_device_kernel_src += $(addprefix ion/src/device/n0100/kernel/drivers/, \
  board.cpp \
  cache.cpp \
  fcc_id.cpp \
  led.cpp \
  power.cpp \
)

ion_device_shared_bootloader_kernel_src += $(addprefix ion/src/device/n0100/shared/drivers/, \
  board.cpp \
  reset.cpp \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
  usb.cpp \
)

ion_device_bootloader_src += ion/src/device/shared/drivers/internal_flash_otp.cpp

ion_device_userland_src += $(addprefix ion/src/device/n0100/shared/drivers/, \
  flash_unprivileged.cpp \
)
