usb_objs += $(addprefix ion/src/device/usb/, \
  calculator.o \
  device.o\
  dfu_interface.o\
  endpoint0.o \
  interface.o\
  request_recipient.o\
  setup_packet.o\
)

usb_objs += $(addprefix ion/src/device/usb/stack/, \
  bos_descriptor.o\
  configuration_descriptor.o \
  descriptor.o\
  device_descriptor.o\
  device_capability_descriptor.o\
  dfu_functional_descriptor.o\
  extended_compat_id_descriptor.o \
  interface_descriptor.o\
  language_id_string_descriptor.o \
  microsoft_os_string_descriptor.o\
  platform_device_capability_descriptor.o\
  streamable.o\
  string_descriptor.o\
  url_descriptor.o\
  webusb_platform_descriptor.o\
)

EPSILON_USB_DFU_XIP ?= 0

ifeq ($(EPSILON_USB_DFU_XIP),1)

objs += ion/src/device/usb/dfu_xip.o
objs += $(usb_objs)

else

dfu_objs += liba/src/assert.o
dfu_objs += liba/src/strlen.o
dfu_objs += liba/src/strlcpy.o
dfu_objs += liba/src/memset.o
dfu_objs += liba/src/memcpy.o
dfu_objs += libaxx/src/cxxabi/pure_virtual.o
dfu_objs += ion/src/device/usb/boot.o
dfu_objs += ion/src/device/keyboard.o
dfu_objs += ion/src/device/device.o
dfu_objs += ion/src/device/usb.o
dfu_objs += ion/src/device/set_msp.o

ion/src/device/usb/dfu.elf: LDFLAGS = --gc-sections -T ion/src/device/usb/dfu.ld
ion/src/device/usb/dfu.elf: $(usb_objs) $(dfu_objs)

ion/src/device/usb/dfu.o: ion/src/device/usb/dfu.bin
	@echo "OBJCOPY $@"
	$(Q) $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata --redefine-sym _binary_ion_src_device_usb_dfu_bin_start=_dfu_bootloader_flash_start --redefine-sym _binary_ion_src_device_usb_dfu_bin_end=_dfu_bootloader_flash_end $< $@

objs += ion/src/device/usb/dfu.o
objs += ion/src/device/usb/dfu_relocated.o

products += $(usb_objs) $(addprefix ion/src/device/usb/dfu, .elf .bin)

endif
