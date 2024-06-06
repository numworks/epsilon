$(call create_module,ion,1,$(addprefix src/shared/, \
  console_line.cpp \
  display_context.cpp \
  events.cpp \
  events_modifier.cpp \
  exam_mode.cpp \
  keyboard_queue.cpp \
  keyboard.cpp \
  layout_events.cpp \
  stack_position.cpp \
  storage/file_system.cpp \
  storage/record.cpp \
  storage/record_name_verifier.cpp \
))

PRIVATE_SFLAGS_ion += \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"

include $(PATH_ion)/shared-$(PLATFORM_TYPE).mak
