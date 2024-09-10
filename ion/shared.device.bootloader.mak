_sources_ion_bootloader := \
shared/dummy/assert.cpp \
$(addprefix device/shared/, \
  boot/rt0.cpp \
  drivers/base64.cpp \
  drivers/flash_information.cpp \
  drivers/serial_number.cpp \
  drivers/usb_unprivileged.cpp \
  post_and_hardware_tests.cpp \
) \
$(addprefix device/epsilon-core/device/shared-core/drivers/, \
  backlight.cpp \
  backlight_basic.cpp \
  battery_$(_ion_mcu_suffix).cpp \
  board_frequency_$(_ion_mcu_suffix).cpp \
  board_peripherals_clocks_$(_ion_mcu_suffix).cpp \
  board_privileged.cpp \
  board_privileged_$(_ion_mcu_suffix).cpp \
  display.cpp \
  display_image.cpp \
  display_shortcut.cpp \
  display_vblank.cpp \
  external_flash.cpp \
  external_flash_qspi_$(_ion_mcu_suffix).cpp \
  flash_authorizations.cpp \
  flash_write.cpp \
  flash_write_with_interruptions.cpp \
  internal_flash.cpp \
  internal_flash_stm32f.cpp:-n0120 \
  internal_flash_stm32h.cpp:+n0120 \
  internal_flash_otp.cpp \
  keyboard.cpp \
  keyboard_$(_ion_mcu_suffix).cpp \
  keyboard_init.cpp \
  option_bytes.cpp \
  option_bytes_$(_ion_mcu_suffix).cpp \
  power_standby_$(_ion_mcu_suffix).cpp \
  random.cpp \
  random_$(_ion_mcu_suffix).cpp \
  reset.cpp \
  swd.cpp \
  timing.cpp \
  usb.cpp \
  usb_gpio_$(PLATFORM).cpp \
  logo_image.png \
) \
$(addprefix device/epsilon-core/device/bootloader/boot/, \
  isr.c \
  rt0.cpp \
) \
$(addprefix device/epsilon-core/device/bootloader/drivers/, \
  authentication.cpp \
  battery.cpp \
  board.cpp \
  board_$(_ion_mcu_suffix).cpp \
  board_ram.cpp \
  cache.cpp \
  display_image.cpp \
  internal_flash_otp_write_$(_ion_mcu_suffix).cpp \
  keyboard.cpp \
  led.cpp \
  power_on_self_test.cpp \
  power_suspend.cpp \
  trampoline.cpp \
  usb.cpp \
  screen_rescue_image.png \
  text_rescue_image.png \
  wire_rescue_image.png \
) \
device/epsilon-core/device/bootloader/main.cpp

ifeq ($(PLATFORM),n0120)
_sources_ion_bootloader += \
  device/epsilon-core/device/shared-core/drivers/board_power_supply_stm32h.cpp \
  device/epsilon-core/device/bootloader/drivers/keyboard_pins_stm32h.cpp
else
_sources_ion_bootloader += \
  device/epsilon-core/device/shared-core/drivers/external_flash_qspi_$(PLATFORM).cpp
endif

_ldflags_ion_bootloader := \
  -Wl,-T,$(PATH_ion)/src/device/epsilon-core/device/bootloader/flash/bootloader_flash.ld \
  -L$(PATH_ion)/src/device/epsilon-core/device/bootloader/flash/$(PLATFORM)

_lddeps_ion_bootloader := \
  $(PATH_ion)/src/device/epsilon-core/device/bootloader/flash/bootloader_flash.ld \

# Libsodium module

$(call import_module,libsodium,$(PATH_ion)/src/device/epsilon-core/external/libsodium)

# Image dependencies
# TODO factor with escher/inliner

_ion_png_serializer := $(PATH_ion)/image/png_serializer.py

# depends_on_compressed_png, <list of cpp>, <list of png>
define depends_on_compressed_png
$(eval \
$(call all_objects_for,$(strip $1)): $(patsubst %.png,$(OUTPUT_DIRECTORY)/%.h,$(strip $2))
$(call all_objects_for,$(strip $1)): SFLAGS += $(foreach d,$(addprefix $(OUTPUT_DIRECTORY)/,$(sort $(dir $(strip $2)))),-I$d)

$(patsubst %.png,$(OUTPUT_DIRECTORY)/%.h,$(strip $2)): $(OUTPUT_DIRECTORY)/%.h: %.png | $$$$(@D)/.
	$$(call rule_label,PNGSER)
	$(PYTHON) $(_ion_png_serializer) --png $$< --header $$@ --cimplementation $$(@:.h=.cpp)

$(patsubst %.png,$(OUTPUT_DIRECTORY)/%.cpp,$(strip $2)): %.cpp: %.h
	@ :
)
endef

$(call depends_on_compressed_png, \
  $(PATH_ion)/src/device/epsilon-core/device/shared-core/drivers/display_image.cpp, \
  $(PATH_ion)/src/device/epsilon-core/device/shared-core/drivers/logo_image.png \
)

$(call depends_on_compressed_png, \
  $(PATH_ion)/src/device/epsilon-core/device/bootloader/drivers/display_image.cpp, \
  $(addprefix $(PATH_ion)/src/device/epsilon-core/device/bootloader/drivers/, \
    screen_rescue_image.png \
    text_rescue_image.png \
    wire_rescue_image.png \
  ) \
)
