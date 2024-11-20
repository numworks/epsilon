ifeq ($(PLATFORM_TYPE),simulator)

# Functions to be exported in the dynamic symbol table
_eadk_exported_functions := \
  eadk_backlight_set_brightness \
  eadk_backlight_brightness \
  eadk_battery_is_charging \
  eadk_battery_level \
  eadk_battery_voltage \
  eadk_display_push_rect \
  eadk_display_push_rect_uniform \
  eadk_display_wait_for_vblank \
  eadk_display_draw_string \
  eadk_event_get \
  eadk_external_data \
  eadk_external_data_size \
  _eadk_keyboard_scan_do_scan \
  _eadk_keyboard_scan_low \
  _eadk_keyboard_scan_high \
  eadk_random \
  eadk_timing_msleep \
  _eadk_timing_millis_low \
  _eadk_timing_millis_high \
  eadk_usb_is_plugged

# Additional exported functions to support newlib in external apps. Their
# implementation is not dummy (unlike device's platform.c) so the app will be
# able to printf to the browser console through them.
# TODO: why is calloc here ? don't we need free and malloc too ?
_eadk_exported_functions += \
  sbrk \
  fstat \
  isatty \
  open \
  read \
  lseek \
  close \
  write \
  calloc

# Functions visible to the external apps that have to be called via asyncify
# (because asyncify cannot guess which functions are asynchrous through the
# dynamic linking boundary).
_eadk_async_functions := \
  eadk_display_wait_for_vblank \
  eadk_event_get \
  _eadk_keyboard_scan_do_scan \
  eadk_timing_msleep

$(call create_module,eadk,1, \
  src/simulator.cpp \
)

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_eadk)): $(KANDINSKY_fonts_dependencies)

else

$(call create_module,eadk,1,$(addprefix src/, \
  platform.c \
  svc.cpp \
  trampoline.cpp \
))

SFLAGS_eadk += \
  -fno-lto \
  -I$(PATH_eadk)/../liba/include \
  -I$(PATH_eadk)/../ion/src/device

LDFLAGS_eadk += -Wl,--relocatable # Make a single .o file
LDFLAGS_eadk += -flinker-output=nolto-rel # Don't output an LTO object
LDFLAGS_eadk += -Wl,--no-gc-sections

$(call create_goal,eadk,eadk)

.PHONY: eadk.o
eadk.o: $(OUTPUT_DIRECTORY)/eadk.o
$(OUTPUT_DIRECTORY)/eadk.o: $(OUTPUT_DIRECTORY)/eadk.elf
	$(call rule_label,STRIP)
	$(Q) arm-none-eabi-strip --strip-debug --discard-all $< -o $@

endif
