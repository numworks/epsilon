ion_device_kernel_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_kernel_src += $(addprefix ion/src/device/shared/drivers/, \
  base64.cpp \
  board_unprivileged.cpp \
  board_unprivileged_standalone.cpp:+n0100 \
  board_unprivileged_dual_slots.cpp:-n0100 \
  flash.cpp:+n0100 \
  flash_unprivileged.cpp:-n0100 \
  serial_number.cpp \
  trampoline.cpp:-n0100 \
  usb_unprivileged.cpp \
)
