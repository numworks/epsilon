usb_src += $(addprefix ion/src/device/shared/usb/, \
  calculator.cpp \
  dfu_interface.cpp\
)

usb_src += $(addprefix ion/src/device/shared/usb/stack/, \
  device.cpp\
  endpoint0.cpp \
  interface.cpp\
  request_recipient.cpp\
  setup_packet.cpp\
  streamable.cpp\
)

usb_src += $(addprefix ion/src/device/shared/usb/stack/descriptor/, \
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

flasher_src += $(addprefix ion/src/device/shared/usb/, \
  flasher.cpp \
)

EPSILON_USB_DFU_XIP ?= 0

ifeq ($(EPSILON_USB_DFU_XIP),1)

ion_device_src += ion/src/device/shared/usb/dfu_xip.cpp
ion_device_src += $(usb_src)

else

dfu_src += liba/src/assert.c
dfu_src += liba/src/strlen.c
dfu_src += liba/src/strlcpy.c
dfu_src += liba/src/memset.c
dfu_src += liba/src/memcpy.c
dfu_src += libaxx/src/cxxabi/pure_virtual.cpp
dfu_src += ion/src/device/shared/usb/boot.cpp
dfu_src += ion/src/device/$(MODEL)/drivers/board.cpp
dfu_src += ion/src/device/$(MODEL)/drivers/cache.cpp
dfu_src += ion/src/device/$(MODEL)/usb/stack/device.cpp
dfu_src += $(addprefix ion/src/device/shared/drivers/, \
  backlight.cpp \
  battery.cpp \
  base64.cpp \
  board.cpp \
  console.cpp \
  crc32.cpp \
  display.cpp \
  events_keyboard_platform.cpp \
  external_flash.cpp \
  flash.cpp \
  keyboard.cpp \
  led.cpp \
  power.cpp\
  random.cpp\
  reset.cpp \
  serial_number.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  wakeup.cpp \
)

$(BUILD_DIR)/ion/src/device/shared/usb/dfu.elf: LDSCRIPT = ion/src/device/shared/usb/dfu.ld
$(BUILD_DIR)/ion/src/device/shared/usb/dfu.elf: $(call object_for,$(usb_src) $(dfu_src))

# In order to link the dfu bootloader inside the epsilon firmware, we need to
# turn the dfu binary (dfu.bin) into an elf object.
# By default, 'objcpy' generates a section 'data' and two symbols to the
# start and the end of the binary input respectively named:
# - '_binary_[file name]_[file extension]_start'
# - '_binary_[file name]_[file extension]_end'.
# For our purpose, dfu.o can go in rodata section and we rename the start and
# end of binary symbols: _dfu_bootloader_flash_[start/end]
$(BUILD_DIR)/ion/src/device/shared/usb/dfu.o: $(BUILD_DIR)/ion/src/device/shared/usb/dfu.bin
	@echo "OBJCOPY $@"
	$(Q) cd $(dir $<) ; $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata.dfu_bootloader --redefine-sym _binary_dfu_bin_start=_dfu_bootloader_flash_start --redefine-sym _binary_dfu_bin_end=_dfu_bootloader_flash_end $(notdir $<) $(notdir $@)

ion_device_src += ion/src/device/shared/usb/dfu.cpp
ion_device_src += ion/src/device/shared/usb/dfu_relocated.cpp

endif
