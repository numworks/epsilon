# USB code

include ion/src/device/shared/usb/Makefile

ion_device_usb_src += $(addprefix ion/src/device/n0100/shared/usb/, \
  calculator_leave.cpp \
)

# DFU code

ion_device_dfu_deps_src += liba/src/strlen.c
ion_device_dfu_deps_src += liba/src/memset.c
ion_device_dfu_deps_src += liba/src/memcpy.c
ion_device_dfu_deps_src += libaxx/src/cxxabi/pure_virtual.cpp

ion_device_dfu_deps_src += $(addprefix ion/src/device/n0100/shared/usb/, \
  timing.cpp \
)

ion_device_dfu_deps_src += $(addprefix ion/src/device/n0100/userland/drivers/, \
  reset.cpp \
  usb.cpp \
)

ion_device_dfu_deps_src += $(addprefix ion/src/device/userland/drivers/, \
  serial_number.cpp \
  usb.cpp \
  reset.cpp \
)

ion_device_dfu_deps_src += $(addprefix ion/src/device/shared/drivers/, \
  base64.cpp \
  internal_flash.cpp \
  serial_number.cpp \
)

ion_device_dfu_deps_src += $(addprefix ion/src/device/n0100/shared/drivers/, \
  flash.cpp \
)

ion_device_dfu_binary_src += $(ion_device_usb_src) $(ion_device_dfu_deps_src)

# Sources required to execute DFU in RAM

# TODO: this could be factorized with ion/src/device/Makefile defining the same flags for the target being built
$(call flavored_object_for,$(ion_device_dfu_binary_src),): SFLAGS += -Iion/src/device/$(MODEL)/shared -Iion/src/device/$(MODEL) -Iion/src/device/shared -Iion/src/device #TODO EMILIE: remove */shared and specify shared in inclusions

$(BUILD_DIR)/ion/src/device/n0100/shared/usb/dfu.elf: LDSCRIPT = ion/src/device/n0100/shared/usb/dfu.ld
$(BUILD_DIR)/ion/src/device/n0100/shared/usb/dfu.elf: LDFLAGS += -Lion/src/device/n0100/shared
$(BUILD_DIR)/ion/src/device/n0100/shared/usb/dfu.elf: LDDEPS += $(LDSCRIPT)
$(BUILD_DIR)/ion/src/device/n0100/shared/usb/dfu.elf: $(call flavored_object_for,$(ion_device_dfu_binary_src),)

# In order to link the dfu bootloader inside the epsilon firmware, we need to
# turn the dfu binary (dfu.bin) into an elf object.
# By default, 'objcpy' generates a section 'data' and two symbols to the
# start and the end of the binary input respectively named:
# - '_binary_[file name]_[file extension]_start'
# - '_binary_[file name]_[file extension]_end'.
# For our purpose, dfu.o can go in rodata section and we rename the start and
# end of binary symbols: _dfu_bootloader_flash_[start/end]

$(BUILD_DIR)/ion/src/device/n0100/shared/usb/dfu.o: $(BUILD_DIR)/ion/src/device/n0100/shared/usb/dfu.bin
	$(call rule_label,OBJCOPY)
	$(Q) cd $(dir $<) ; $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata.dfu_bootloader --redefine-sym _binary_dfu_bin_start=_dfu_bootloader_flash_start --redefine-sym _binary_dfu_bin_end=_dfu_bootloader_flash_end $(notdir $<) $(notdir $@)

ion_device_dfu_src += ion/src/device/n0100/shared/usb/dfu.cpp
ion_device_dfu_src += ion/src/device/n0100/shared/usb/dfu_relocated.cpp
