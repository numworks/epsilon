$(call assert_defined,ION_layout_variant)

$(call create_module,ion,1, $(patsubst %, test/%:+test, \
  crc32.cpp \
  events.cpp \
  exam_bytes.cpp \
  exam_mode.cpp \
  keyboard.cpp \
  storage.cpp  \
))

_ion_display_width_epsilon = 320
_ion_display_height_epsilon = 240
_ion_display_border_epsilon = 0
_ion_display_width_scandium = 200
_ion_display_height_scandium = 87
_ion_display_border_scandium = 3

_ion_keyboard_columns = 6
_ion_keyboard_rows_epsilon = 9
_ion_keyboard_rows_scandium = 8

SFLAGS_ion += \
  -I$(PATH_ion)/include/ion/keyboard/$(ION_layout_variant) \
  -DION_DISPLAY_WIDTH=$(_ion_display_width_$(ION_layout_variant)) \
  -DION_DISPLAY_HEIGHT=$(_ion_display_height_$(ION_layout_variant)) \
  -DION_DISPLAY_BORDER=$(_ion_display_border_$(ION_layout_variant)) \
  -DION_KEYBOARD_COLUMNS=$(_ion_keyboard_columns) \
  -DION_KEYBOARD_ROWS=$(_ion_keyboard_rows_$(ION_layout_variant)) \

PRIVATE_SFLAGS_ion += \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"
  -DION_KEYBOARD_RICH=$(if $(findstring epsilon,$(ION_layout_variant)),1,0)

ION_LOG_EVENTS_NAME ?= $(DEBUG)

ifneq ($(ION_LOG_EVENTS_NAME),0)
SFLAGS_ion += -DION_LOG_EVENTS_NAME=1
endif

ifeq ($(PLATFORM),u0-discovery)
# TODO: rework how ion is tied to scandium
include $(PATH_scandium_ion)/u0.mak
else
include $(PATH_ion)/shared.$(PLATFORM_TYPE).mak
endif

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_ion)): $(KANDINSKY_fonts_dependencies)
