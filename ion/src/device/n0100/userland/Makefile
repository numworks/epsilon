include ion/src/device/userland/Makefile

ion_device_userland_src += $(addprefix ion/src/device/n0100/userland/drivers/, \
  reset.cpp \
)

ion_device_userland_src += $(addprefix ion/src/shared/dummy/, \
  external_apps.cpp \
)

$(call object_for,$(ion_device_userland_svc_src)): SFLAGS += -fno-lto

ion_device_userland_src += $(ion_device_userland_svc_src)

