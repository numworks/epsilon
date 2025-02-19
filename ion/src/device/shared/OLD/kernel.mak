ion_device_kernel_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_kernel_src += $(addprefix ion/src/device/shared/drivers/, \
  assert.cpp:+kernelassert \
  base64.cpp \
  board_unprivileged.cpp \
  console_display.cpp:+kernelassert \
  flash_information.cpp\
  serial_number.cpp \
  trampoline.cpp \
  usb_unprivileged.cpp \
)
