ifeq ($(FIRMWARE_COMPONENT),bootloader)
include epsilon-bootloader/device/Makefile
else
-include ion/src/device/$(FIRMWARE_COMPONENT)/Makefile
endif

include ion/src/device/shared/Makefile
include ion/src/device/$(MODEL)/Makefile

# We need to work around a GCC bug (concerning versions < 5.1). It is valid in
# C++11 to initialize a character array by providing a string litteral (e.g.
# char test[4]= "ab"; is valid and should initialize test to 'a','b',0,0).
# Older versions of GCC are not conformant so we resort to an initializer list.
initializer_list = $(shell echo $(1) | sed  "s/\(.\)/'\1',/g")0
$(call object_for,ion/src/device/kernel/drivers/kernel_header.cpp ion/src/device/userland/drivers/userland_header.cpp): SFLAGS += -DPATCH_LEVEL="$(call initializer_list,$(PATCH_LEVEL))" -DEPSILON_VERSION="$(call initializer_list,$(EPSILON_VERSION))"

ION_DEVICE_SFLAGS += -Iion/src/device/$(MODEL)/shared -Iion/src/device/$(MODEL) -Iion/src/device/shared -Iion/src/device #TODO EMILIE: remove */shared and specify shared in inclusions

IN_FACTORY ?= 0
ION_DEVICE_SFLAGS += -DPCB_LATEST=$(PCB_LATEST) -DIN_FACTORY=$(IN_FACTORY)

$(call object_for,$(ion_device_$(FIRMWARE_COMPONENT)_src)): SFLAGS += $(ION_DEVICE_SFLAGS)

