include ion/src/device/shared/usb/Makefile

ion_device_flasher_src += $(addprefix ion/src/device/shared/boot/, \
  rt0.cpp \
)

ion_device_flasher_src += $(addprefix ion/src/device/shared/drivers/, \
  base64.cpp \
  serial_number.cpp \
  usb_unprivileged.cpp \
)

ion_device_flasher_src += $(ion_device_dfu_src)
