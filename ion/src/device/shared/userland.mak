include ion/src/device/shared/usb/Makefile

ion_device_userland_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/shared/drivers/, \
  assert.cpp \
  board_unprivileged.cpp \
  console_display.cpp:+consoledisplay \
  flash_unprivileged.cpp \
  trampoline.cpp \
  usb_unprivileged.cpp \
)

ion_device_userland_src += $(ion_device_dfu_src)
ion_device_userland_src += ion/src/device/shared/post_and_hardware_tests
