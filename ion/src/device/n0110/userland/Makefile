include ion/src/device/userland/Makefile

ion_device_userland_svc_src += $(addprefix ion/src/device/n0110/userland/drivers/, \
  flash.cpp \
  reset.cpp \
  serial_number.cpp \
  usb.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/n0110/userland/drivers/, \
  board.cpp \
  external_apps.cpp \
)

$(call object_for,$(ion_device_userland_svc_src)): SFLAGS += -fno-lto

ion_device_userland_src += $(ion_device_userland_svc_src)

$(call object_for,ion/src/device/n0110/userland/drivers/external_apps.cpp): SFLAGS += -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

