include ion/src/device/userland/Makefile

ion_device_userland_src += $(addprefix ion/src/device/n0110/userland/drivers/, \
  board.cpp \
  external_apps.cpp \
  flash.cpp \
)

$(call object_for,ion/src/device/n0110/userland/drivers/external_apps.cpp): SFLAGS += -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

