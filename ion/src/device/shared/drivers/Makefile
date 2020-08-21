ion_device_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  battery.cpp \
  base64.cpp \
  board_privileged.cpp:+privileged \
  board_unprivileged.cpp:-privileged \
  console_uart.cpp:+consoleuart \
  console_display.cpp:+consoledisplay \
  console_dummy.cpp:-consoledisplay \
  console_dummy.cpp:-consoleuart \
  cortex_control.s \
  crc32.cpp \
  display.cpp \
  events_keyboard_platform.cpp \
  exam_mode.cpp \
  external_flash.cpp \
  flash.cpp \
  internal_flash.cpp \
  keyboard.cpp \
  led.cpp \
  power.cpp\
  power_unprivileged.cpp:-privileged \
  power_privileged.cpp:+privileged \
  random.cpp\
  reset_privileged.cpp:+privileged \
  reset_unprivileged.cpp:-privileged \
  serial_number.cpp \
  svcall_handler.cpp:+svcallhandler \
  swd.cpp \
  timing.cpp \
  timing_privileged.cpp:+privileged \
  usb.cpp \
  wakeup.cpp \
)
