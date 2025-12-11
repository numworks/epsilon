$(call assert_defined,ION_variant)

$(call create_module,ion,1, $(patsubst %, src/test/%:+test, \
  crc32.cpp \
  events.cpp \
  keyboard.cpp \
))

_ion_display_width_epsilon = 320
_ion_display_height_epsilon = 240
_ion_display_border_epsilon = 0
_ion_display_border_color_epsilon = KDColor::RGB888(0, 0, 0)
_ion_display_width_scandium = 200
_ion_display_height_scandium = 87
_ion_display_border_scandium = 3
_ion_display_border_color_scandium = KDColor::RGB888(0x88, 0x96, 0x8C)

_ion_keyboard_columns = 6
_ion_keyboard_rows_epsilon = 9
_ion_keyboard_rows_scandium = 7

SFLAGS_ion += \
  -I$(PATH_ion)/include/ion/keyboard/$(ION_variant) \
  -DION_DISPLAY_WIDTH=$(_ion_display_width_$(ION_variant)) \
  -DION_DISPLAY_HEIGHT=$(_ion_display_height_$(ION_variant)) \
  -DION_DISPLAY_BORDER=$(_ion_display_border_$(ION_variant)) \
  -DION_DISPLAY_BORDER_COLOR="$(_ion_display_border_color_$(ION_variant))" \
  -DION_KEYBOARD_COLUMNS=$(_ion_keyboard_columns) \
  -DION_KEYBOARD_ROWS=$(_ion_keyboard_rows_$(ION_variant)) \
  -DION_KEYBOARD_HAS_ALPHA=$(if $(findstring epsilon,$(ION_variant)),1,0)

PRIVATE_SFLAGS_ion += \
  -DSOFTWARE_VERSION=\"$(APP_VERSION)\" \
  -DCOMMIT_HASH=\"$(COMMIT_HASH)\" \
  -DION_VARIANT_$(call upper,$(ION_variant))

ION_STORAGE_LOG ?= 0
ION_LOG_EVENTS_NAME ?= 0
ifeq ($(PLATFORM_TYPE),simulator)
ifeq ($(DEBUG),1)
ION_STORAGE_LOG := 1
ION_LOG_EVENTS_NAME := 1
endif
endif

ifneq ($(ION_STORAGE_LOG),0)
SFLAGS_ion += -DION_STORAGE_LOG=1
endif

ifneq ($(ION_LOG_EVENTS_NAME),0)
SFLAGS_ion += -DION_LOG_EVENTS_NAME=1
endif

ifeq ($(PLATFORM_TYPE),simulator)
include $(PATH_ion)/shared.simulator.mak
else ifneq ($(filter $(PLATFORM),n0200 u0-discovery),)
include $(PATH_ion)/scandium.shared.mak
else
include $(PATH_ion)/epsilon.$(PLATFORM_TYPE).mak
endif

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_ion)): $(KANDINSKY_fonts_dependencies)
