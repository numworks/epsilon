include ion/src/device/n0110/shared/usb/Makefile

# Shared

## bench/bootloader/flasher/kernel

ion_device_bench_bootloader_flasher_kernel_src = $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  base64.cpp \
  board_privileged.cpp \
  display.cpp \
  internal_flash_otp.cpp \
  reset.cpp \
  serial_number.cpp \
  timing.cpp \
  usb_privileged.cpp \
)

ion_device_bench_bootloader_flasher_kernel_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board_privileged.cpp \
  usb.cpp \
)

ion_device_bench_bootloader_flasher_kernel_src += ion/src/device/shared/boot/rt0.cpp

## bootloader/flasher/kernel/userland

ion_device_bootloader_flasher_kernel_userland_src = $(addprefix ion/src/device/shared/drivers/, \
  usb.cpp \
  usb_unprivileged.cpp \
)

## bench/bootloader/flasher

ion_device_bench_bootloader_flasher_src = $(addprefix ion/src/device/n0110/shared/drivers/, \
  external_flash.cpp \
  external_flash_command.cpp \
)

## bench/bootloader/kernel

ion_device_bench_bootloader_kernel_src = $(addprefix ion/src/device/shared/drivers/, \
  battery.cpp \
  keyboard.cpp \
)

ion_device_bench_bootloader_kernel_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  power_configuration.cpp \
)

## bench/bootloader

ion_device_bench_bootloader_src = $(addprefix ion/src/device/n0110/shared/drivers/, \
  display_shortcut.cpp \
  display_vblank.cpp \
  keyboard_init.cpp \
  power_suspend.cpp \
)

## bench/flasher

ion_device_bench_flasher_src = $(addprefix ion/src/device/n0110/shared/, \
  boot/isr.c \
  boot/rt0.cpp \
  drivers/board_unprotected.cpp \
)

## bench/kernel

ion_device_bench_kernel_src = $(addprefix ion/src/device/shared/drivers/, \
  backlight_advanced.cpp \
  battery_charge.cpp \
  board_frequency.cpp \
  crc32.cpp \
  led.cpp \
  power.cpp \
  wakeup.cpp \
)

ion_device_bench_kernel_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  cache.cpp \
)

## kernel/userland

ion_device_kernel_userland_src = $(addprefix ion/src/device/shared/drivers/, \
  board_unprivileged.cpp \
)

ion_device_kernel_userland_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  board_unprivileged.cpp \
  trampoline.cpp \
)

# Bench

ion_device_bench_src += $(ion_device_bench_bootloader_flasher_kernel_src) $(ion_device_bench_bootloader_flasher_src) $(ion_device_bench_bootloader_kernel_src) $(ion_device_bench_bootloader_src) $(ion_device_bench_flasher_src) $(ion_device_bench_kernel_src)
ion_device_bench_src += ion/src/device/shared/post_and_hardware_tests

# Bootloader

ion_device_bootloader_src += $(ion_device_bench_bootloader_flasher_kernel_src) $(ion_device_bootloader_flasher_kernel_userland_src) $(ion_device_bench_bootloader_flasher_src) $(ion_device_bench_bootloader_kernel_src) $(ion_device_bench_bootloader_userland_src) $(ion_device_bench_bootloader_src) $(ion_device_dfu_src)
ion_device_bootloader_src += ion/src/device/shared/post_and_hardware_tests

ion_device_bootloader_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight_basic.cpp \
  display_image.cpp \
  internal_flash.cpp \
  random.cpp \
)

ion_device_bootloader_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
)

# Flasher

ion_device_flasher_src += $(ion_device_bench_bootloader_flasher_kernel_src) $(ion_device_bootloader_flasher_kernel_userland_src) $(ion_device_bench_bootloader_flasher_src) $(ion_device_bench_flasher_src) $(ion_device_dfu_src)

ion_device_flasher_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight_basic.cpp \
  internal_flash.cpp \
)

# Kernel

ion_device_kernel_src += $(ion_device_bench_bootloader_flasher_kernel_src) $(ion_device_bootloader_flasher_kernel_userland_src) $(ion_device_bench_bootloader_kernel_src) $(ion_device_bench_kernel_src) $(ion_device_kernel_userland_src)

ion_device_kernel_src += $(addprefix ion/src/device/shared/drivers/, \
  internal_flash.cpp \
  random.cpp \
)

ion_device_kernel_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
)

# Userland

ion_device_userland_src += $(ion_device_bootloader_flasher_kernel_userland_src) $(ion_device_bench_bootloader_userland_src) $(ion_device_kernel_userland_src) $(ion_device_dfu_src)
ion_device_userland_src += ion/src/device/shared/boot/rt0.cpp ion/src/device/shared/post_and_hardware_tests

ion_device_userland_src += $(addprefix ion/src/device/n0110/shared/drivers/, \
  flash_unprivileged.cpp \
)
