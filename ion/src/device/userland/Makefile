ion_device_userland_src += $(addprefix ion/src/device/userland/boot/, \
  isr.cpp \
  rt0.cpp \
  rt0_standalone.cpp:+n0100 \
  rt0_bootloader.cpp:-n0100 \
)

ion_device_userland_src += $(addprefix ion/src/device/userland/drivers/, \
  board_dual_slots.cpp:-n0100 \
  board_ram_dfu_relocated.cpp:+n0100 \
  board_ram_dfu_xip.cpp:-n0100 \
  clipboard.cpp \
  console_display.cpp:+consoledisplay \
  console_dummy.cpp:-consoledisplay \
  external_apps.cpp:+allow3rdparty \
  reset.cpp \
  reset_n0100.cpp:+n0100 \
  serial_number.cpp \
  stack.cpp \
  usb_n0100.cpp:+n0100 \
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
  flash.cpp:-n0100 \
  keyboard.cpp \
  led.cpp \
  pcb_version.cpp \
  persisting_bytes.cpp \
  power.cpp \
  random.cpp \
  reset_svcall.cpp:-n0100 \
  serial_number_svcall.cpp:-n0100 \
  timing.cpp \
  usb.cpp \
  usb_svcall.cpp:-n0100 \
)

ion_device_userland_src += $(ion_device_userland_svc_src)
ion_device_userland_src += ion/src/shared/collect_registers.cpp

$(call object_for,$(ion_device_userland_svc_src)): SFLAGS += -fno-lto

$(call object_for,ion/src/device/userland/drivers/external_apps.cpp): SFLAGS += -DEXTERNAL_APPS_API_LEVEL=$(EXTERNAL_APPS_API_LEVEL)

ION_DEVICE_SFLAGS += -Iion/src/device/userland
