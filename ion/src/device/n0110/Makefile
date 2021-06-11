ion_device_bootloader_src += $(addprefix ion/src/device/n0110/bootloader/drivers/, \
  trampoline.cpp \
  authentication.cpp \
  board.cpp \
  external_flash.cpp \
  external_flash_command.cpp \
  flash_privileged.cpp \
  option_bytes.cpp \
  power.cpp \
)

ion_device_shared_bootloader_kernel_src += ion/src/device/shared/drivers/internal_flash_otp.cpp

ion_device_kernel_src += $(addprefix ion/src/device/n0110/kernel/drivers/, \
  board.cpp \
  cache.cpp \
  external_flash.cpp \
  fcc_id.cpp \
  flash_privileged.cpp \
  led.cpp \
  power.cpp \
)

ion_device_shared_bootloader_kernel_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board.cpp \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
  power.cpp \
  reset.cpp \
  usb.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board.cpp \
  flash_unprivileged.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/n0110/userland/drivers/, \
  board.cpp \
)
