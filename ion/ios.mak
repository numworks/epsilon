SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  shared/apple/platform_images.mm \
  shared/apple/platform_language.mm \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/keyboard_callback.cpp \
  shared/dummy/window_callback.cpp \
  shared/haptics.cpp \
  shared/circuit_breaker.cpp \
  shared/clipboard_helper_sdl.cpp \
  shared/dummy/journal.cpp \
)

# TODO collect_registers.cpp depends on arch, tweak haussmann
SOURCES_ion += $(PATH_ion)/src/shared/collect_registers.cpp

_ion_simulator_files := 0
