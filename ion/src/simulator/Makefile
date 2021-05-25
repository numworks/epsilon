ion_src += $(addprefix ion/src/shared/dummy/, \
  authentication.cpp \
  backlight.cpp \
  battery.cpp \
  display.cpp \
  external_apps.cpp \
  fcc_id.cpp \
  led.cpp \
  platform_info.cpp \
  post_and_hardware_tests.cpp \
  power.cpp \
  reset.h \
  stack.cpp \
  usb.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  clipboard.cpp \
  console.cpp \
  crc32.cpp \
  display.cpp \
  events.cpp \
  events_platform.cpp \
  framebuffer.cpp \
  keyboard.cpp \
  layout.cpp \
  main.cpp \
  persisting_bytes.cpp \
  random.cpp \
  timing.cpp \
  window.cpp \
)

ion_simulator_assets = background.jpg horizontal_arrow.png vertical_arrow.png round.png small_squircle.png large_squircle.png
ion_simulator_assets_paths = $(add_prefix ion/src/simulator/assets/,$(ion_simulator_assets))

include ion/src/simulator/$(TARGET)/Makefile
include ion/src/simulator/external/Makefile

SFLAGS += -DION_EVENTS_JOURNAL

# This flags the ability to store files on the local system
# These include state-files and cached SDL window postion
ifeq ($(ION_SIMULATOR_FILES),1)
ion_src += $(addprefix ion/src/simulator/shared/, \
  actions.cpp \
  state_file.cpp \
  screenshot.cpp \
	window_pos_cached.cpp \
	platform_files.cpp \
)
SFLAGS += -DION_SIMULATOR_FILES=1
else
ion_src += ion/src/simulator/shared/dummy/window_pos.cpp
endif
