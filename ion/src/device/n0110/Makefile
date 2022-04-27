ion_device_src += $(addprefix ion/src/device/n0110/drivers/, \
  cache.cpp \
  external_flash.cpp \
  led.cpp \
  power.cpp \
  reset.cpp \
  usb.cpp \
)

ifeq ($(filter bootloader, ${MAKECMDGOALS}), bootloader)
ion_device_src += $(addprefix bootloader/drivers/, \
  board.cpp \
)
ion_device_src += $(addprefix bootloader/boot/, \
  rt0.cpp \
)
else
ion_device_src += $(addprefix ion/src/device/n0110/drivers/, \
  board.cpp \
)
ion_device_src += $(addprefix ion/src/device/n0110/boot/, \
  rt0.cpp \
)
endif

ion_device_src += $(addprefix ion/src/device/n0110/, \
  platform_info.cpp \
)

LDSCRIPT ?= ion/src/device/n0110/flash.ld
