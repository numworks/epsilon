$(call assert_defined,ION_LAYOUT_VARIANT)

$(call create_module,ion,1, $(patsubst %, test/%:+test, \
  crc32.cpp \
  events.cpp \
  exam_bytes.cpp \
  exam_mode.cpp \
  keyboard.cpp \
  storage.cpp  \
))

SFLAGS_ion += -I$(PATH_ion)/include/ion/keyboard/$(ION_LAYOUT_VARIANT)

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
