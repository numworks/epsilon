# Makefile belows should augment $(ion_device_src)
include epsilon-bootloader/device/Makefile
include ion/src/device/shared/Makefile
include ion/src/device/bench/Makefile
include ion/src/device/kernel/Makefile
include ion/src/device/flasher/Makefile
include ion/src/device/userland/Makefile
include ion/src/device/$(MODEL)/Makefile

# We need to work around a GCC bug (concerning versions < 5.1). It is valid in
# C++11 to initialize a character array by providing a string litteral (e.g.
# char test[4]= "ab"; is valid and should initialize test to 'a','b',0,0).
# Older versions of GCC are not conformant so we resort to an initializer list.
initializer_list = $(shell echo $(1) | sed  "s/\(.\)/'\1',/g")0
$(call object_for,ion/src/device/kernel/drivers/kernel_header.cpp ion/src/device/userland/drivers/userland_header.cpp): SFLAGS += -DPATCH_LEVEL="$(call initializer_list,$(PATCH_LEVEL))" -DEPSILON_VERSION="$(call initializer_list,$(EPSILON_VERSION))"

ion_device_bootloader_src += $(ion_device_shared_bootloader_kernel_src)
ion_device_kernel_src += $(ion_device_shared_bootloader_kernel_src)

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/shared/telemetry_console.cpp
endif

ion_device_userland_src += ion/src/shared/collect_registers.cpp

ION_DEVICE_SFLAGS += -Iion/src/device/$(MODEL)/shared -Iion/src/device/$(MODEL) -Iion/src/device/shared -Iion/src/device #TODO EMILIE: remove */shared and specify shared in inclusions

ION_DEVICE_FLASHER_SFLAGS += -Iion/src/device/$(MODEL)/flasher -Iion/src/device/flasher

ION_DEVICE_KERNEL_SFLAGS += -Iion/src/device/$(MODEL)/kernel -Iion/src/device/kernel

ION_DEVICE_USERLAND_SFLAGS += -Iion/src/device/userland

$(call object_for,$(ion_device_bootloader_src)): SFLAGS += $(ION_DEVICE_SFLAGS) $(ION_DEVICE_BOOTLOADER_SFLAGS)
$(call object_for,$(ion_device_flasher_src)): SFLAGS += $(ION_DEVICE_SFLAGS) $(ION_DEVICE_FLASHER_SFLAGS)
$(call object_for,$(ion_device_kernel_src)): SFLAGS += $(ION_DEVICE_SFLAGS) $(ION_DEVICE_KERNEL_SFLAGS)
$(call object_for,$(ion_device_userland_src)): SFLAGS += $(ION_DEVICE_SFLAGS) $(ION_DEVICE_USERLAND_SFLAGS)

IN_FACTORY ?= 0
ION_DEVICE_SFLAGS += -DPCB_LATEST=$(PCB_LATEST) -DIN_FACTORY=$(IN_FACTORY)

ion_src += $(ion_device_src)
