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
  interface_descriptor.o\
  language_id_string_descriptor.o \
  microsoft_os_platform_descriptor.o\
  platform_device_capability_descriptor.o\
  streamable.o\
  string_descriptor.o\
  url_descriptor.o\
  webusb_platform_descriptor.o\
)

USB_DFU_XIP_FLASH := 0
ifeq ($(USB_DFU_XIP_FLASH),1)

objs += ion/src/device/usb_dfu_xip_flash.o
objs += $(usb_objs)

else

usb_objs += liba/src/assert.o
usb_objs += liba/src/strlen.o
usb_objs += liba/src/strlcpy.o
usb_objs += liba/src/memset.o
usb_objs += liba/src/memcpy.o
usb_objs += libaxx/src/cxxabi/pure_virtual.o
usb_objs += ion/src/device/usb/boot.o
usb_objs += ion/src/device/keyboard.o
usb_objs += ion/src/device/device.o
usb_objs += ion/src/device/usb.o

ion/src/device/usb/dfu.elf: LDFLAGS = --gc-sections -T ion/src/device/boot/dfu.ld
ion/src/device/usb/dfu.elf: $(usb_objs)

ion/src/device/usb/dfu.o: ion/src/device/usb/dfu.bin
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata --redefine-sym _binary_ion_src_device_usb_dfu_bin_start=_dfu_bootloader_flash_start --redefine-sym _binary_ion_src_device_usb_dfu_bin_end=_dfu_bootloader_flash_end $< $@

objs += ion/src/device/usb/dfu.o
objs += ion/src/device/usb_dfu_relocated_ram.o

products += $(usb_objs) $(addprefix ion/src/device/usb/dfu, .elf .bin)

endif
