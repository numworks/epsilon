# Makefile belows should augment $(ion_device_src)
include ion/src/device/shared/Makefile
include ion/src/device/bench/Makefile
include ion/src/device/bootloader/Makefile
include ion/src/device/flasher/Makefile
include ion/src/device/$(MODEL)/Makefile

$(call object_for,ion/src/shared/platform_info.cpp): SFLAGS += -DHEADER_SECTION="__attribute__((section(\".header\")))"

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/shared/telemetry_console.cpp
endif

ion_src += ion/src/shared/collect_registers.cpp

ION_DEVICE_SFLAGS += -Iion/src/device/$(MODEL)/shared -Iion/src/device/shared
ION_DEVICE_BOOTLOADER_SFLAGS += -Iion/src/device/$(MODEL)/bootloader -Iion/src/device/bootloader

$(call object_for,$(ion_device_src) $(ion_device_flasher_src) $(ion_device_bench_src) $(ion_device_n0110_bootloader_permanent_src) $(ion_device_n0110_bootloader_updatable_src) $(ion_device_n0110_external_flash_src)): SFLAGS += $(ION_DEVICE_SFLAGS) // TODO Emilie remove

$(call object_for,$(ion_device_bootloader_src)): SFLAGS += $(ION_DEVICE_SFLAGS) $(ION_DEVICE_BOOTLOADER_SFLAGS)

ion_src += $(ion_device_src)
