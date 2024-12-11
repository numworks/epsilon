_ion_firmware_components := bench bootloader flasher kernel userland

ifeq ($(PLATFORM),n0120)
_ion_mcu_suffix := stm32h
else
ifneq ($(filter $(PLATFORM),n0110 n0115),)
_ion_mcu_suffix := stm32f
else
$(error Unsupported device platform $(PLATFORM))
endif
endif

-include $(patsubst %,$(PATH_ion)/shared.device.%.mak,$(_ion_firmware_components))

# USB sources - begin

_sources_ion_usb := $(addprefix device/shared/usb/, \
$(addprefix stack/, \
  device.cpp \
  endpoint0.cpp \
  interface.cpp \
  request_recipient.cpp \
  setup_packet.cpp \
  streamable.cpp \
) \
$(addprefix stack/descriptor/, \
  bos_descriptor.cpp \
  configuration_descriptor.cpp \
  descriptor.cpp \
  device_descriptor.cpp \
  device_capability_descriptor.cpp \
  dfu_functional_descriptor.cpp \
  extended_compat_id_descriptor.cpp \
  interface_descriptor.cpp \
  language_id_string_descriptor.cpp \
  microsoft_os_string_descriptor.cpp \
  platform_device_capability_descriptor.cpp \
  string_descriptor.cpp \
  url_descriptor.cpp \
  webusb_platform_descriptor.cpp \
) \
  calculator_bootloader.cpp:+bootloader \
  calculator_flasher_$(_ion_mcu_suffix).cpp:+flasher \
  calculator_userland.cpp:+userland \
  calculator_userland_leave.cpp:+userland:+allow3rdparty \
  calculator_userland_leave_reset.cpp:+userland:-allow3rdparty \
  dfu_interface.cpp \
)

_sources_ion_usb := $(addsuffix :-kernel:-bench,$(_sources_ion_usb))

# USB sources - end

SOURCES_ion += $(addprefix $(PATH_ion)/src/, \
  $(foreach c,$(_ion_firmware_components),$(addsuffix :+$c,$(_sources_ion_$c))) \
  $(_sources_ion_usb) \
)

# TODO Of those flags, phase out those that can be edited on the command line,
# namely DEVELOPMENT, IN_FACTORY and EMBED_EXTRA_DATA
PRIVATE_SFLAGS_ion += \
  -DDEVELOPMENT=$(DEVELOPMENT) \
  -DEMBED_EXTRA_DATA=$(EMBED_EXTRA_DATA) \
  -DIN_FACTORY=$(IN_FACTORY) \
  -DPCB_LATEST=$(PCB_LATEST) \
  -DSIGNATURE_INDEX=$(SIGNATURE_INDEX) \

PRIVATE_SFLAGS_ion += \
  -I$(PATH_ion)/src/device/epsilon-core/device/include/$(PLATFORM) \
  -I$(PATH_ion)/src/device/include/$(PLATFORM) \
  -I$(PATH_ion)/src/device \
  -I$(PATH_ion)/src/device/shared \
  -I$(PATH_ion)/src/device/epsilon-core/device \
  -I$(PATH_ion)/src/device/epsilon-core/device/shared-core

LDFLAGS_ion += \
  $(foreach c,$(_ion_firmware_components),$(addsuffix :+$c,$(_ldflags_ion_$c))) \
  -L$(PATH_ion)/src/device/epsilon-core/device/shared-core/flash \
  -L$(PATH_ion)/src/device/shared/flash \
  -L$(OUTPUT_DIRECTORY)/$(PATH_ion)/src/device/epsilon-core/device/shared-core/flash \
  -L$(OUTPUT_DIRECTORY)/$(PATH_ion)/src/device/shared/flash

# Prevent building kernel or userland without a slot.
LDFLAGS_ion += \
 -Wl,-T,$(PATH_ion)/src/device/shared/flash/missing_slot.ld:+kernel:-A:-B \
 -Wl,-T,$(PATH_ion)/src/device/shared/flash/missing_slot.ld:+userland:-A:-B

LDDEPS_ion += \
  $(foreach c,$(_ion_firmware_components),$(addsuffix :+$c,$(_lddeps_ion_$c))) \
  $(OUTPUT_DIRECTORY)/$(PATH_ion)/src/device/shared/flash/board.ld

$(OUTPUT_DIRECTORY)/$(PATH_ion)/src/device/shared/flash/board.ld: $(PATH_ion)/src/device/include/$(PLATFORM)/config/board.h | $$(@D)/.
	$(call rule_label,AWK)
	$(CXX) $(SFLAGS) -E $< -o $(@:.ld=.h)
	awk '/^constexpr/ {$$1=$$2=""; sub(";.*", ";"); print}; /^static_assert/ {sub("static_assert", "ASSERT"); print}' $(@:.ld=.h) >$@
