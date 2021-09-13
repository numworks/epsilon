# USB code

ion_device_usb_src += $(addprefix ion/src/device/shared/usb/, \
  calculator.cpp \
  calculator_leave_bootloader.cpp:+leavebootloader,+n0110 \
  calculator_leave_n0100.cpp:+n0100 \
  calculator_leave_n0110.cpp:-leavebootloader,-leaveuserland,+n0110 \
  calculator_leave_userland.cpp:+leaveuserland,+n0110 \
  dfu_interface.cpp \
  dfu_relocated.cpp:+n0100 \
  dfu_xip.cpp:+n0110 \
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

ifeq ($(MODEL),n0100)
include ion/src/device/shared/usb/dfu_relocated.mak
endif

ion_device_dfu_src += $(ion_device_usb_src)
