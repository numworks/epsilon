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

$(usb_src): SFLAGS += $(ION_DEVICE_SFLAGS)

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
dfu_src += $(addprefix ion/src/device/shared/drivers/, \
  base64.cpp \
  external_flash.cpp \
  flash.cpp \
  keyboard.cpp \
  reset.cpp \
  serial_number.cpp \
  timing.cpp \
  usb.cpp \
)

$(BUILD_DIR)/ion/src/device/shared/usb/dfu.elf: LDSCRIPT = ion/src/device/shared/usb/dfu.ld
$(BUILD_DIR)/ion/src/device/shared/usb/dfu.elf: $(call object_for,$(usb_src) $(dfu_src))

$(BUILD_DIR)/ion/src/device/shared/usb/dfu.o: $(BUILD_DIR)/ion/src/device/shared/usb/dfu.bin
	@echo "OBJCOPY $@"
	$(Q) cd $(dir $<) ; $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata --redefine-sym _binary_dfu_bin_start=_dfu_bootloader_flash_start --redefine-sym _binary_dfu_bin_end=_dfu_bootloader_flash_end $(notdir $<) $(notdir $@)

ion_device_src += ion/src/device/shared/usb/dfu.cpp
ion_device_src += ion/src/device/shared/usb/dfu_relocated.cpp

endif
