SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  macos/platform_files.mm \
  shared/apple/platform_images.mm \
  shared/apple/platform_language.mm \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/keyboard_callback.cpp \
  shared/dummy/window_callback.cpp \
  shared/unix/platform_files.cpp \
  shared/haptics.cpp \
  shared/circuit_breaker.cpp \
  shared/clipboard_helper_sdl.cpp \
  shared/journal.cpp \
)

# TODO collect_registers.cpp depends on arch, tweak haussmann
SOURCES_ion += $(PATH_ion)/src/shared/collect_registers.cpp

_ion_simulator_window_setup := $(PATH_ion)/src/simulator/macos/window.mm
_ion_simulator_files := 1
_ion_external_apps := 1

_ion_simulator_icons_sizes := 16x16 32x32 64x64 128x128 256x256 512x512 1024x1024
_ion_simulator_iconset := $(OUTPUT_DIRECTORY)/app/assets/app.iconset
_ion_simulator_icons_use_mask := 1

include $(PATH_ion)/shared.apple.mak

$(eval $(foreach a,$(_ion_simulator_assets),\
$(call rule_for_simulator_resource,COPY,$(notdir $a),$a,\
  cp $$^ $$@ \
)))

$(call rule_for_simulator_resource, \
  ICNUTIL,app.icns,$(_ion_simulator_icons), \
  iconutil --convert icns --output $$@ $$(dir $$<) \
)
