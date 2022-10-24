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

ion_simulator_assets = background.webp backgroundlea.jpg horizontal_arrow.png vertical_arrow.png round.png small_squircle.png large_squircle.png
ion_simulator_assets_paths = $(addprefix ion/src/simulator/assets/,$(ion_simulator_assets))

$(addprefix $(BUILD_DIR),ion/src/simulator/assets/background.jpg): ion/src/simulator/assets/background.webp
	$(call rule_label,CONVERT)
	$(Q) mkdir -p $(dir $@)
	$(Q) convert $< $@

simulator_app_deps += $(addprefix $(BUILD_DIR),ion/src/simulator/assets/background.jpg)

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
  platform_files.cpp \
)
SFLAGS += -DION_SIMULATOR_FILES=1
ION_SIMULATOR_WINDOW_SETUP ?= ion/src/simulator/shared/window_position_cached.cpp
else
ION_SIMULATOR_WINDOW_SETUP ?= ion/src/simulator/shared/dummy/window_position.cpp
endif
ion_src += $(ION_SIMULATOR_WINDOW_SETUP)
