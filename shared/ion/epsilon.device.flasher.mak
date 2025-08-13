_sources_ion_flasher := \
$(addprefix shared/, \
  dummy/assert.cpp \
  display_context.cpp \
) \
$(addprefix device/shared/, \
  boot/rt0.cpp \
  drivers/base64.cpp \
  drivers/dfu_interfaces.cpp \
  drivers/serial_number.cpp \
  drivers/usb_unprivileged.cpp \
) \
$(addprefix device/core/device/shared-core/boot/, \
  isr.c \
  rt0_start.cpp \
) \
$(addprefix device/core/device/shared-core/drivers/, \
  backlight.cpp \
  backlight_basic.cpp \
  board_frequency_$(_ion_mcu_suffix).cpp \
  board_peripherals_clocks_$(_ion_mcu_suffix).cpp \
  board_privileged.cpp \
  board_privileged_$(_ion_mcu_suffix).cpp \
  board_unprotected.cpp \
  cache.cpp \
  display.cpp \
  external_flash.cpp \
  external_flash_qspi_$(_ion_mcu_suffix).cpp \
  external_flash_no_sscg.cpp \
  flash_authorizations.cpp \
  flash_write_with_interruptions.cpp \
  internal_flash.cpp \
  internal_flash_$(_ion_mcu_suffix).cpp \
  internal_flash_otp.cpp \
  option_bytes_$(_ion_mcu_suffix).cpp \
  option_bytes.cpp \
  reset.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  usb_gpio_$(PLATFORM).cpp \
) \
$(addprefix device/core/device/flasher/drivers/, \
  board.cpp \
  board_ram.cpp \
  flash_write.cpp \
  flash_information.cpp \
  led.cpp \
  reset.cpp \
  usb.cpp \
) \
  device/core/device/flasher/main.cpp

ifneq ($(PLATFORM),n0120)
_sources_ion_flasher += \
  device/core/device/shared-core/drivers/external_flash_qspi_$(PLATFORM).cpp
endif

# _linker_script := ram.ld
_linker_script := signable_ram.ld

_ldflags_ion_flasher := \
  -Wl,-T,$(PATH_ion)/src/device/core/device/flasher/$(_linker_script)

_lddeps_ion_flasher := \
  $(PATH_ion)/src/device/core/device/flasher/$(_linker_script)

ifneq ($(EMBED_EXTRA_DATA),0)
_sources_ion_flasher += trampoline.o bootloader.o

$(call generated_sources_for, $(PATH_ion)/src/trampoline.o): $(call generated_sources_for, $(PATH_ion)/src/bootloader.o)
	@ :

$(call generated_sources_for, $(PATH_ion)/src/bootloader.o): $(call generated_sources_for, bootloader/bootloader.elf)
	$(call rule_label, OBJCOPY)
	$(Q) $(OBJCOPY) -O binary -S -R .trampoline -R .pseudo_otp -R .unused_flash $< $(@:.o=.bin)
	$(Q) $(OBJCOPY) -O binary -S -j .trampoline $< $(@:bootloader.o=trampoline.bin)
	$(Q) $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.bootloader $(@:.o=.bin) $@
	$(Q) $(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.trampoline $(@:bootloader.o=trampoline.bin) $(@:bootloader.o=trampoline.o)

endif

