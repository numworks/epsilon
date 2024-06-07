_ion_firmware_components := bench bootloader flasher kernel userland

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
  calculator_userland.cpp:+userland \
  calculator_userland_leave.cpp:+userland:+allow3rdparty \
  calculator_userland_leave_reset.cpp:+userland:-allow3rdparty \
  dfu_interface.cpp \
)
ifeq ($(PLATFORM),n0120)
_sources_ion_usb += device/shared/usb/calculator_flasher_stm32h.cpp:+flasher
else
_sources_ion_usb += device/shared/usb/calculator_flasher_stm32f.cpp:+flasher
endif

_sources_ion_usb := $(addsuffix :-kernel:-bench,$(_sources_ion_usb))

# USB sources -end

SOURCES_ion += $(addprefix $(PATH_ion)/src/, \
  $(foreach c,$(_ion_firmware_components),$(addsuffix :+$c,$(_sources_ion_$c))) \
  $(_sources_ion_usb) \
)

# TODO Of those flags, phase out those that can be edited on the command line,
# namely DEVELOPMENT and IN_FACTORY
PRIVATE_SFLAGS_ion += \
  -DDEVELOPMENT=$(DEVELOPMENT) \
  -DIN_FACTORY=$(IN_FACTORY) \
  -DPCB_LATEST=$(PCB_LATEST) \
  -DSIGNATURE_INDEX=$(SIGNATURE_INDEX)

PRIVATE_SFLAGS_ion += \
  -I$(PATH_ion)/src/device/include/$(PLATFORM) \
  -I$(PATH_ion)/src/device

LDFLAGS_ion += \
  $(foreach c,$(_ion_firmware_components),$(addsuffix :+$c,$(_ldflags_ion_$c))) \
  -L$(PATH_ion)/src/device/shared/flash \
  -L$(OUTPUT_DIRECTORY)/ion/src/device/shared/flash

LDDEPS_ion += \
  $(foreach c,$(_ion_firmware_components),$(addsuffix :+$c,$(_lddeps_ion_$c))) \
  $(OUTPUT_DIRECTORY)/$(PATH_ion)/src/device/shared/flash/board.ld

$(OUTPUT_DIRECTORY)/$(PATH_ion)/src/device/shared/flash/board.ld: $(PATH_ion)/src/device/include/$(PLATFORM)/config/board.h | $$(@D)/.
	$(call rule_label,AWK)
	$(QUIET) $(CXX) $(SFLAGS) -E $< -o $(@:.ld=.h)
	$(QUIET) awk '/^constexpr/ {$$1=$$2=""; sub(";.*", ";"); print}; /^static_assert/ {sub("static_assert", "ASSERT"); print}' $(@:.ld=.h) >$@

