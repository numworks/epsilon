# Shared
ion_device_n0110_bootloader_kernel_src = $(addprefix ion/src/device/n0110/shared/drivers/, \
  board.cpp \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
  power.cpp \
  reset.cpp \
  usb.cpp \
)

# Target specific
ion_device_bootloader_src += $(ion_device_n0110_bootloader_kernel_src)

ion_device_flasher_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board.cpp \
  reset.cpp \
  usb.cpp \
)

ion_device_kernel_src += $(ion_device_n0110_bootloader_kernel_src)
ion_device_kernel_src += $(addprefix ion/src/device/n0110/kernel/drivers/, \
  board.cpp \
  cache.cpp \
  external_flash.cpp \
  fcc_id.cpp \
  flash_privileged.cpp \
  led.cpp \
  power.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board.cpp \
  flash_unprivileged.cpp \
)
ion_device_userland_src += $(addprefix ion/src/device/n0110/userland/drivers/, \
  board.cpp \
)
