usb_src += $(addprefix ion/src/device/usb/, \
  calculator.cpp \
  dfu_interface.cpp \
)

usb_src += $(addprefix ion/src/device/usb/stack/, \
  device.cpp \
  endpoint0.cpp \
  interface.cpp \
  request_recipient.cpp \
  setup_packet.cpp \
  streamable.cpp \
)

usb_src += $(addprefix ion/src/device/usb/stack/descriptor/, \
  bos_descriptor.cpp \
  configuration_descriptor.cpp \
  descriptor.cpp \
  device_capability_descriptor.cpp \
  device_descriptor.cpp \
  dfu_functional_descriptor.cpp \
  extended_compat_id_descriptor.cpp \
  interface_descriptor.cpp \
  language_id_string_descriptor.cpp \
  microsoft_os_string_descriptor.cpp \
  platform_device_capability_descriptor.cpp \
  string_descriptor.cpp \
  url_descriptor.cpp \
  webusb_platform_descriptor.cpp \
)

EPSILON_USB_DFU_XIP ?= 0

ifeq ($(EPSILON_USB_DFU_XIP),1)

src += ion/src/device/usb/dfu_xip.cpp
src += $(usb_src)

else

dfu_src += liba/src/assert.cpp
dfu_src += liba/src/strlen.cpp
dfu_src += liba/src/strlcpy.cpp
dfu_src += liba/src/memset.cpp
dfu_src += liba/src/memcpy.cpp
dfu_src += libaxx/src/cxxabi/pure_virtual.cpp
dfu_src += ion/src/device/usb/boot.cpp
dfu_src += ion/src/device/keyboard.cpp
dfu_src += ion/src/device/device.cpp
dfu_src += ion/src/device/usb.cpp
dfu_src += ion/src/device/base64.cpp
dfu_src += ion/src/device/flash.cpp
dfu_src += ion/src/device/timing.cpp

$(BUILD_DIR)/ion/src/device/usb/dfu.elf: LDSCRIPT = ion/src/device/usb/dfu.ld
$(BUILD_DIR)/ion/src/device/usb/dfu.elf: $(call object_for,$(usb_src)) $(call object_for,$(dfu_src))

# This command embeds a binary file into an object one.
# This allows us to embed standalone code (the dfu routines) into the final
# executable, and easily relocate it to RAM for execution. The objcopy command
# that turns binary data into an ELF object generates three symbols (start, size
# and end), but prefixes them with a mangled file path. To have consistent names
# we simply "cd" into the directory. This assumes input and output lives in the
# same directory.
$(BUILD_DIR)/ion/src/device/usb/dfu.o: $(BUILD_DIR)/ion/src/device/usb/dfu.bin
	@echo "OBJCOPY $@"
	$(Q) cd $(dir $<) ; $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata --redefine-sym _binary_dfu_bin_start=_dfu_bootloader_flash_start --redefine-sym _binary_dfu_bin_end=_dfu_bootloader_flash_end $(notdir $<) $(notdir $@)

src += ion/src/device/usb/dfu.cpp
src += ion/src/device/usb/dfu_relocated.cpp

endif
