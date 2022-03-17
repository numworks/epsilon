ion_src += $(addprefix ion/src/shared/dummy/, \
  backlight.cpp \
  board.cpp \
  battery.cpp \
  display.cpp \
  exam_mode.cpp \
  fcc_id.cpp \
  led.cpp \
  pcb_version.cpp \
  power.cpp \
  serial_number.cpp \
  stack.cpp \
  usb.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  clipboard.cpp \
  console.cpp \
  rtc.cpp \
  crc32.cpp \
  display.cpp \
  events.cpp \
  events_platform.cpp \
  framebuffer.cpp \
  keyboard.cpp \
  layout.cpp \
  main.cpp \
  platform_info.cpp \
  random.cpp \
  timing.cpp \
  window.cpp \
)

ion_simulator_assets = background.jpg horizontal_arrow.png vertical_arrow.png round.png small_squircle.png large_squircle.png
ion_simulator_assets_paths = $(add_prefix ion/src/simulator/assets/,$(ion_simulator_assets))

include ion/src/simulator/$(TARGET)/Makefile
include ion/src/simulator/external/Makefile

SFLAGS += -DION_EVENTS_JOURNAL

ifeq ($(ION_SIMULATOR_FILES),1)
ion_src += $(addprefix ion/src/simulator/shared/, \
  actions.cpp \
  state_file.cpp \
)
SFLAGS += -DION_SIMULATOR_FILES=1
endif
