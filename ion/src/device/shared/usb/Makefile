# USB code

ion_device_usb_src += $(addprefix ion/src/device/shared/usb/, \
  calculator.cpp \
  dfu_interface.cpp\
)

ion_device_usb_src += $(addprefix ion/src/device/shared/usb/stack/, \
  device.cpp\
  endpoint0.cpp \
  interface.cpp\
  request_recipient.cpp\
  setup_packet.cpp\
  streamable.cpp\
)

ion_device_usb_src += $(addprefix ion/src/device/shared/usb/stack/descriptor/, \
  bos_descriptor.cpp\
  configuration_descriptor.cpp \
  descriptor.cpp\
  device_descriptor.cpp\
  device_capability_descriptor.cpp\
  dfu_functional_descriptor.cpp\
  extended_compat_id_descriptor.cpp \
  interface_descriptor.cpp\
  language_id_string_descriptor.cpp \
  microsoft_os_string_descriptor.cpp\
  platform_device_capability_descriptor.cpp\
  string_descriptor.cpp\
  url_descriptor.cpp\
  webusb_platform_descriptor.cpp\
)

# DFU code

ion_device_dfu_src += liba/src/abs.c
ion_device_dfu_src += liba/src/assert.c
ion_device_dfu_src += liba/src/strlen.c
ion_device_dfu_src += liba/src/strlcpy.c
ion_device_dfu_src += liba/src/memset.c
ion_device_dfu_src += liba/src/memcpy.c
ion_device_dfu_src += libaxx/src/cxxabi/pure_virtual.cpp
ion_device_dfu_src += ion/src/device/shared/usb/boot.cpp
ion_device_dfu_src += ion/src/device/$(MODEL)/drivers/board.cpp
ion_device_dfu_src += ion/src/device/$(MODEL)/drivers/cache.cpp
ion_device_dfu_src += ion/src/device/$(MODEL)/drivers/external_flash.cpp
ion_device_dfu_src += ion/src/device/$(MODEL)/drivers/reset.cpp
ion_device_dfu_src += ion/src/device/$(MODEL)/drivers/usb.cpp
ion_device_dfu_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  battery.cpp \
  base64.cpp \
  board.cpp \
  console_uart.cpp \
  crc32.cpp \
  display.cpp \
  events_keyboard_platform.cpp \
  flash.cpp \
  internal_flash.cpp \
  keyboard.cpp \
  led.cpp \
  power.cpp\
  random.cpp\
  reset.cpp \
  serial_number.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  usb_desc.cpp \
  wakeup.cpp \
)

# Sources required to execute DFU in place
ion_device_src += ion/src/device/shared/usb/dfu_xip.cpp:+usbxip
ion_device_src += $(addsuffix :+usbxip,$(ion_device_usb_src))

# Sources required to execute DFU in RAM

$(BUILD_DIR)/ion/src/device/shared/usb/dfu.elf: LDSCRIPT = ion/src/device/shared/usb/dfu.ld
$(BUILD_DIR)/ion/src/device/shared/usb/dfu.elf: $(call object_for,$(ion_device_usb_src) $(ion_device_dfu_src))

# In order to link the dfu bootloader inside the epsilon firmware, we need to
# turn the dfu binary (dfu.bin) into an elf object.
# By default, 'objcpy' generates a section 'data' and two symbols to the
# start and the end of the binary input respectively named:
# - '_binary_[file name]_[file extension]_start'
# - '_binary_[file name]_[file extension]_end'.
# For our purpose, dfu.o can go in rodata section and we rename the start and
# end of binary symbols: _dfu_bootloader_flash_[start/end]
$(BUILD_DIR)/ion/src/device/shared/usb/dfu.o: $(BUILD_DIR)/ion/src/device/shared/usb/dfu.bin
	$(call rule_label,OBJCOPY)
	$(Q) cd $(dir $<) ; $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata.dfu_bootloader --redefine-sym _binary_dfu_bin_start=_dfu_bootloader_flash_start --redefine-sym _binary_dfu_bin_end=_dfu_bootloader_flash_end $(notdir $<) $(notdir $@)

ion_device_src += ion/src/device/shared/usb/dfu.cpp:-usbxip
ion_device_src += ion/src/device/shared/usb/dfu_relocated.cpp:-usbxip

ion_device_src += $(addprefix ion/src/device/shared/drivers/, \
  usb_desc.cpp \
)
