_sources_ion_flasher := \
$(addprefix shared/, \
  dummy/assert.cpp \
  display_context.cpp \
) \
$(addprefix device/shared/, \
  boot/rt0.cpp \
  drivers/base64.cpp \
  drivers/serial_number.cpp \
  drivers/usb_unprivileged.cpp \
) \
$(addprefix device/epsilon-core/device/shared-core/boot/, \
  isr.c \
  rt0_start.cpp \
) \
$(addprefix device/epsilon-core/device/shared-core/drivers/, \
  backlight.cpp \
  backlight_basic.cpp \
  board_frequency_$(_ion_mcu_suffix).cpp \
  board_peripherals_clocks_$(_ion_mcu_suffix).cpp \
  board_privileged.cpp \
  board_privileged_$(_ion_mcu_suffix).cpp \
  board_unprotected.cpp \
  cache.cpp \
  display.cpp \
  external_flash.cpp \
  external_flash_qspi_$(_ion_mcu_suffix).cpp \
  flash_authorizations.cpp \
  flash_write_with_interruptions.cpp \
  internal_flash.cpp \
  internal_flash_stm32f.cpp:-n0120 \
  internal_flash_stm32h.cpp:+n0120 \
  internal_flash_otp.cpp \
  reset.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  usb_gpio_$(PLATFORM).cpp \
) \
$(addprefix device/epsilon-core/device/flasher/drivers/, \
  board.cpp \
  board_ram.cpp \
  flash_write.cpp \
  flash_information.cpp \
  reset.cpp \
  usb.cpp \
) \
  device/epsilon-core/device/flasher/main.cpp

ifneq ($(PLATFORM),n0120)
_sources_ion_flasher += \
  device/epsilon-core/device/shared-core/drivers/external_flash_qspi_$(PLATFORM).cpp
endif

_ldflags_ion_flasher := \
  -Wl,-T,$(PATH_ion)/src/device/epsilon-core/device/flasher/ram.ld

_lddeps_ion_flasher := \
  $(PATH_ion)/src/device/epsilon-core/device/flasher/ram.ld
