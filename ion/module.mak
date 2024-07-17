$(call create_module,ion,1, $(patsubst %, test/%:+test, \
  crc32.cpp \
  events.cpp \
  exam_bytes.cpp \
  exam_mode.cpp \
  keyboard.cpp \
  storage.cpp  \
))

PRIVATE_SFLAGS_ion += \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"

include $(PATH_ion)/shared.$(PLATFORM_TYPE).mak
