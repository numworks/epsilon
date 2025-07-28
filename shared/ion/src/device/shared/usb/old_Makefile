# USB code

ion_device_usb_src += $(addprefix ion/src/device/shared/usb/, \
  calculator.cpp \
  calculator_leave_bootloader.cpp:+leavebootloader \
  calculator_leave_flasher_stm32f.cpp:-leavebootloader,-leaveuserland,-n0120 \
  calculator_leave_flasher_stm32h.cpp:-leavebootloader,-leaveuserland,+n0120 \
  calculator_leave_reset.cpp:+leaveuserland,-allow3rdparty \
  calculator_leave_userland.cpp:+leaveuserland,+allow3rdparty \
  dfu_interface.cpp \
  dfu_xip.cpp \
)

ion_device_usb_src += $(addprefix ion/src/device/shared/usb/stack/, \
  device.cpp \
  endpoint0.cpp \
  interface.cpp \
  request_recipient.cpp \
  setup_packet.cpp \
  streamable.cpp \
)

ion_device_usb_src += $(addprefix ion/src/device/shared/usb/stack/descriptor/, \
  bos_descriptor.cpp \
  configuration_descriptor.cpp \
  descriptor.cpp \
  device_descriptor.cpp \
  device_capability_descriptor.cpp \
  dfu_functional_descriptor.cpp \
  extended_compat_id_descriptor.cpp \
  interface_descriptor.cpp \
  language_id_string_descriptor.cpp \
  microsoft_os_string_descriptor.cpp \
  platform_device_capability_descriptor.cpp \
  string_descriptor.cpp \
  url_descriptor.cpp \
  webusb_platform_descriptor.cpp \
)

ion_device_dfu_src += $(ion_device_usb_src)
