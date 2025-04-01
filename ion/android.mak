SOURCES_ion += $(addprefix  $(PATH_ion)/src/simulator/android/src/cpp/, \
  haptics_enabled.cpp \
  platform_images.cpp \
  platform_language.cpp \
)

SOURCES_ion += $(addprefix  $(PATH_ion)/src/simulator/shared/, \
  dummy/journal.cpp \
  dummy/keyboard_callback.cpp \
  dummy/window_callback.cpp \
  circuit_breaker.cpp \
  clipboard_helper_sdl.cpp \
  haptics.cpp \
)

SOURCES_ion += $(addprefix $(PATH_ion)/src/shared/, \
  collect_registers.cpp \
)

$(call all_objects_for,$(PATH_ion)/src/simulator/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

LDFLAGS_ion += -ljnigraphics -llog

_ion_simulator_files := 0
