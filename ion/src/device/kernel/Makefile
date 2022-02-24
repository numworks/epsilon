ion_device_kernel_src = $(addprefix ion/src/device/kernel/, \
  boot/isr.c \
  boot/rt0.cpp \
  drivers/authentication.cpp \
  drivers/battery.cpp \
  drivers/board.cpp \
  drivers/circuit_breaker.cpp \
  drivers/display.cpp \
  drivers/events.cpp \
  drivers/kernel_header.cpp \
  drivers/keyboard.cpp \
  drivers/led.cpp \
  drivers/wakeup.cpp \
  drivers/persisting_bytes.cpp \
  drivers/power.cpp \
  drivers/svcall_handler.cpp \
  drivers/svcall_handler_as.s \
  drivers/swd.cpp \
  drivers/timing.cpp \
  drivers/usb.cpp \
  drivers/wakeup.cpp \
  main.cpp \
  warning_display.cpp \
)

ION_DEVICE_SFLAGS = -Iion/src/device/$(MODEL)/kernel -Iion/src/device/kernel
