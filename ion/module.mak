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
_ion_display_width_scandium = 200
_ion_display_height_scandium = 87

SFLAGS_ion += \
  -I$(PATH_ion)/include/ion/keyboard/$(ION_layout_variant) \
  -DION_DISPLAY_WIDTH=$(_ion_display_width_$(ION_layout_variant)) \
  -DION_DISPLAY_HEIGHT=$(_ion_display_height_$(ION_layout_variant))

PRIVATE_SFLAGS_ion += \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"

ION_LOG_EVENTS_NAME ?= $(DEBUG)

ifneq ($(ION_LOG_EVENTS_NAME),0)
SFLAGS_ion += -DION_LOG_EVENTS_NAME=1
endif

include $(PATH_ion)/shared.$(PLATFORM_TYPE).mak

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_ion)): $(KANDINSKY_fonts_dependencies)
