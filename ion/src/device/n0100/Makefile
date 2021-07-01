# Shared
ion_device_n0100_bootloader_kernel_src = $(addprefix ion/src/device/n0100/shared/drivers/, \
  board.cpp \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
  reset.cpp \
  usb.cpp \
)

# Target specific
ion_device_bootloader_src += $(ion_device_n0100_bootloader_kernel_src)

ion_device_kernel_src += $(ion_device_n0100_bootloader_kernel_src)
ion_device_kernel_src += $(addprefix ion/src/device/n0100/kernel/drivers/, \
  board.cpp \
  cache.cpp \
  fcc_id.cpp \
  led.cpp \
  power.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/n0100/shared/drivers/, \
  flash_unprivileged \
)
