ion_device_userland_src += $(addprefix ion/src/device/userland/boot/, \
  isr.cpp \
  rt0.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/userland/drivers/, \
  board_dual_slots.cpp \
  board_ram_dfu_xip.cpp \
  clipboard.cpp \
  console_display.cpp:+consoledisplay \
  console_dummy.cpp:-consoledisplay \
  external_apps.cpp:+allow3rdparty \
  reset.cpp \
  serial_number.cpp \
  stack.cpp \
  userland_header.cpp \
)

ion_device_userland_src += $(addprefix ion/src/shared/dummy/, \
  external_apps.cpp:-allow3rdparty \
)

ion_device_userland_svc_src += $(addprefix ion/src/device/userland/drivers/, \
  authentication.cpp \
  backlight.cpp \
  battery.cpp \
  board.cpp \
  circuit_breaker.cpp \
  crc32.cpp \
  display.cpp \
  events.cpp \
  fcc_id.cpp \
  flash.cpp \
  keyboard.cpp \
  led.cpp \
  pcb_version.cpp \
  persisting_bytes.cpp \
  power.cpp \
  random.cpp \
  reset_svcall.cpp \
  serial_number_svcall.cpp \
  timing.cpp \
  usb.cpp \
  usb_svcall.cpp \
)

ion_device_userland_src += $(ion_device_userland_svc_src)
ion_device_userland_src += ion/src/shared/collect_registers.cpp

$(call object_for,$(ion_device_userland_svc_src)): SFLAGS += -fno-lto

$(call object_for,ion/src/device/userland/drivers/external_apps.cpp): SFLAGS += -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

ION_DEVICE_SFLAGS += -Iion/src/device/userland
