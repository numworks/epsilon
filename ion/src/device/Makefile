# Makefile belows should augment $(ion_device_src)
include ion/src/device/shared/Makefile
include ion/src/device/bench/Makefile
include ion/src/device/flasher/Makefile
include ion/src/device/$(MODEL)/Makefile

$(call object_for,ion/src/device/n0100/platform_info.cpp ion/src/device/n0110/platform_info.cpp ion/src/device/bootloader/platform_info.cpp): SFLAGS += -DHEADER_SECTION="__attribute__((section(\".header\")))"

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/shared/telemetry_console.cpp
endif

ion_device_src += ion/src/shared/collect_registers.cpp

IN_FACTORY ?= 0

ION_DEVICE_SFLAGS = -Iion/src/device/$(MODEL) -Iion/src/device/shared -DPCB_LATEST=$(PCB_LATEST) -DIN_FACTORY=$(IN_FACTORY)

$(call object_for,$(ion_device_src) $(ion_device_flasher_src) $(ion_device_bench_src)): SFLAGS += $(ION_DEVICE_SFLAGS)

ion_src += $(ion_device_src)
