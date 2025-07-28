include ion/src/device/shared/usb/Makefile

ion_device_bootloader_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_bootloader_src += $(addprefix ion/src/device/shared/drivers/, \
  base64.cpp \
  flash_information.cpp \
  serial_number.cpp \
  usb_unprivileged.cpp \
)

ion_device_bootloader_src += ion/src/device/shared/post_and_hardware_tests
ion_device_bootloader_src += $(ion_device_dfu_src)
