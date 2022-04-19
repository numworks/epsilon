ion_device_updater_src += $(addprefix ion/src/device/shared/boot/, \
  isr.c \
  rt0.cpp \
)

ion_device_updater_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  backlight_basic.cpp \
  board_frequency_stm32f.cpp:-n0120 \
  board_frequency_stm32h.cpp:+n0120 \
  board_privileged.cpp \
  cache.cpp \
  display.cpp \
  external_flash.cpp \
  external_flash_qspi_n0110.cpp:+n0110 \
  external_flash_qspi_n0120.cpp:+n0120 \
  flash_privileged.cpp \
  flash_unprivileged.cpp \
  internal_flash.cpp \
  internal_flash_otp_dual_slots.cpp \
  option_bytes.cpp \
  option_bytes_n0110.cpp:+n0110 \
  option_bytes_n0120.cpp:+n0120 \
  reset.cpp \
  swd.cpp \
  timing.cpp \
)
