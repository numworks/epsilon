include ion/src/device/shared/usb/Makefile

# Shared

ion_device_bootloader_flasher_kernel_src = $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/backlight.cpp \
  drivers/base64.cpp \
  drivers/board_privileged.cpp \
  drivers/display.cpp \
  drivers/internal_flash.cpp \
  drivers/internal_flash_otp.cpp \
  drivers/reset.cpp \
  drivers/serial_number.cpp \
  drivers/timing.cpp \
  drivers/usb.cpp \
  drivers/usb_privileged.cpp \
  drivers/usb_unprivileged.cpp \
)

## bench/flasher

ion_device_bench_flasher = $(addprefix ion/src/device/n0110/shared/boot/, \
  isr.c \
  rt0.cpp \
)

## bootloader/kernel

ion_device_bootloader_kernel_src = $(addprefix ion/src/device/shared/drivers/, \
  battery.cpp \
  keyboard.cpp \
  random.cpp \
)

ion_device_bootloader_kernel_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board_privileged.cpp \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
  power.cpp \
  reset.cpp \
  usb.cpp \
)

ion_device_bootloader_kernel_src += $(ion_device_bootloader_flasher_kernel_src)

## bootloader/flasher

ion_device_bootloader_flasher_src = $(addprefix ion/src/device/n0110/shared/drivers/, \
  external_flash.cpp \
  external_flash_command.cpp \
)

ion_device_bootloader_flasher_src += $(ion_device_bootloader_flasher_kernel_src)

# kernel/userland

ion_device_kernel_userland_src = $(addprefix ion/src/device/shared/drivers/, \
  board_unprivileged.cpp \
)

ion_device_kernel_userland_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board_unprivileged.cpp \
  trampoline.cpp \
)

# Bench

ion_device_bench_src += ion/src/device/shared/boot/rt0.cpp

ion_device_bench_src += $(addprefix ion/src/device/shared/drivers/, \
  board_privileged.cpp \
  display.cpp \
)

ion_device_bench_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board_privileged.cpp \
)

ion_device_bench_src += $(ion_device_bench_flasher)

# Bootloader

ion_device_bootloader_src += $(addprefix ion/src/device/shared/, \
  drivers/backlight_basic.cpp \
  drivers/display_image.cpp \
  post_and_hardware_tests.cpp \
 )

ion_device_bootloader_src += $(ion_device_bootloader_kernel_src) $(ion_device_bootloader_flasher_src) $(ion_device_dfu_src)

# Kernel

ion_device_kernel_src += $(addprefix ion/src/device/shared/drivers/, \
  led.cpp \
)

ion_device_kernel_src += $(ion_device_bootloader_kernel_src) $(ion_device_kernel_userland_src)

# Flasher

ion_device_flasher_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight_basic.cpp \
)

ion_device_flasher_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board_privileged.cpp \
  reset.cpp \
  usb.cpp \
)

ion_device_flasher_src += $(ion_device_bench_flasher) $(ion_device_bootloader_flasher_src) $(ion_device_dfu_src)

# Userland

ion_device_userland_src += $(addprefix ion/src/device/shared/, \
  boot/rt0.cpp \
  drivers/usb.cpp \
  drivers/usb_unprivileged.cpp \
  post_and_hardware_tests.cpp \
)

ion_device_userland_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  flash_unprivileged.cpp \
)

ion_device_userland_src += $(ion_device_kernel_userland_src) $(ion_device_dfu_src)
