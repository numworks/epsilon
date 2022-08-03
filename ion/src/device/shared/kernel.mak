ion_device_kernel_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_kernel_src += $(addprefix ion/src/device/shared/drivers/, \
  base64.cpp \
  board_unprivileged.cpp \
  flash_unprivileged.cpp\
  serial_number.cpp \
  trampoline.cpp \
  usb_unprivileged.cpp \
)
