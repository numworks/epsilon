# Makefile belows should augment $(ion_device_src)
include ion/src/device/shared/Makefile
include ion/src/device/bench/Makefile
include ion/src/device/bootloader/Makefile
include ion/src/device/kernel/Makefile
include ion/src/device/flasher/Makefile
include ion/src/device/signature/Makefile
include ion/src/device/userland/Makefile
include ion/src/device/$(MODEL)/Makefile

ion_device_bootloader_src += $(ion_device_shared_bootloader_kernel_src)
ion_device_kernel_src += $(ion_device_shared_bootloader_kernel_src)

$(call object_for,ion/src/shared/platform_info.cpp): SFLAGS += -DHEADER_SECTION="__attribute__((section(\".header\")))"

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/shared/telemetry_console.cpp
endif

ion_device_userland_src += ion/src/shared/collect_registers.cpp

ION_DEVICE_SFLAGS += -Iion/src/device/$(MODEL)/shared -Iion/src/device/$(MODEL) -Iion/src/device/shared -Iion/src/device #TODO EMILIE: remove */shared and specify shared in inclusions

ION_DEVICE_BOOTLOADER_SFLAGS += -Iion/src/device/$(MODEL)/bootloader -Iion/src/device/bootloader

ION_DEVICE_KERNEL_SFLAGS += -Iion/src/device/$(MODEL)/kernel -Iion/src/device/kernel

ION_DEVICE_USERLAND_SFLAGS += -Iion/src/device/userland

$(call object_for,$(ion_device_bootloader_src)): SFLAGS += $(ION_DEVICE_SFLAGS) $(ION_DEVICE_BOOTLOADER_SFLAGS)
$(call object_for,$(ion_device_kernel_src)): SFLAGS += $(ION_DEVICE_SFLAGS) $(ION_DEVICE_KERNEL_SFLAGS)
$(call object_for,$(ion_device_userland_src)): SFLAGS += $(ION_DEVICE_SFLAGS) $(ION_DEVICE_USERLAND_SFLAGS)

# Add libsodium dependency
$(call object_for,$(libsodium_src)): SFLAGS += -Iion/src/external/libsodium/include/sodium -Iion/src/external/libsodium/include/device -DCONFIGURED=1

ion_device_bootloader_src += $(libsodium_src)
ion_device_kernel_src += $(libsodium_src)

ion_src += $(ion_device_src)
