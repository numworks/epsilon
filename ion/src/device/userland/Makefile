ion_device_userland_src += $(addprefix ion/src/device/userland/, \
  boot/entry_point.c \
  boot/rt0.cpp \
  drivers/backlight.cpp \
  drivers/battery.cpp \
  drivers/display.cpp \
  drivers/console_dummy.cpp:-consoledisplay \
  drivers/console_display.cpp:+consoledisplay \
  drivers/events.cpp \
  drivers/events_keyboard_platform.cpp \
  drivers/keyboard.cpp \
  drivers/persisting_bytes.cpp \
  drivers/timing.cpp \
  drivers/usb.cpp \
)

