# USB code

include ion/src/device/shared/usb/Makefile

ion_device_usb_src += $(addprefix ion/src/device/n0110/shared/usb/, \
  calculator_leave_bootloader.cpp:+leavebootloader \
  calculator_leave_userland.cpp:+leaveuserland \
)

# DFU code

ion_device_dfu_src += ion/src/device/n0110/shared/usb/dfu_xip.cpp
ion_device_dfu_src += $(ion_device_usb_src)
