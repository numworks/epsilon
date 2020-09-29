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
  exam_mode_privileged.cpp:+privileged \
  exam_mode_unprivileged.cpp:-privileged \
  external_flash.cpp \
  flash.cpp \
  internal_flash.cpp \
  keyboard.cpp \
  led.cpp \
  led_privileged.cpp:+privileged \
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

# Extract the Ion::Device::Timing::MillisElapsed and Ion::Device::LED::sLedColor variable addresses to link it in the external flash
$(BUILD_DIR)/ion/src/device/shared/drivers/bootloader_updatable.sym: $(BUILD_DIR)/bootloader.updatable.elf
	$(eval TIMING_MILLIS_ELAPSED_ADDRESS := $(shell arm-none-eabi-nm $^ | grep "_ZN3Ion6Device6Timing13MillisElapsedE" | cut -d ' ' -f 1))
	$(eval LED_COLOR_ADDRESS := $(shell arm-none-eabi-nm $^ | grep "_ZN3Ion6Device3LED9sLedColorE" | cut -d ' ' -f 1))
	echo "_ZN3Ion6Device6Timing13MillisElapsedE = 0x$(TIMING_MILLIS_ELAPSED_ADDRESS);" >> $@
	echo "_ZN3Ion6Device3LED9sLedColorE = 0x$(LED_COLOR_ADDRESS);" >> $@
