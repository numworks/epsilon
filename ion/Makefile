GIT := $(shell command -v git 2> /dev/null)
PATCH_LEVEL = NONE
ifdef GIT
  PATCH_LEVEL = $(shell git rev-parse HEAD | head -c 7)
endif

SFLAGS += -Iion/include -DKD_CONFIG_H=1

include ion/image/Makefile
include ion/src/$(PLATFORM)/Makefile
-include ion/test/$(PLATFORM)/Makefile
include ion/src/shared/tools/Makefile

ion_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  display_context.cpp \
  events.cpp \
  events_modifier.cpp \
  exam_mode.cpp \
  keyboard.cpp \
  keyboard_queue.cpp \
  layout_events.cpp \
  stack_position.cpp \
  storage/file_system.cpp \
  storage/record_name_verifier.cpp \
  storage/record.cpp \
)

# TODO EMILIE: tidy makefiles: these following should go in ion/src/device/Makefile?

ion_device_bench_src += $(addprefix ion/src/shared/, \
  dummy/assert.cpp \
  console_line.cpp \
  display_context.cpp \
  layout_events.cpp \
)

ion_device_flasher_src += $(addprefix ion/src/shared/, \
  dummy/assert.cpp \
  display_context.cpp \
)

ion_device_kernel_src += $(addprefix ion/src/shared/, \
  console_line.cpp:+kernelassert \
  display_context.cpp:+kernelassert \
  events.cpp \
  events_modifier.cpp \
  keyboard.cpp \
  keyboard_queue.cpp \
  layout_events.cpp \
)

ion_device_bootloader_src += $(addprefix ion/src/shared/, \
  dummy/assert.cpp \
)

ion_device_userland_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  display_context.cpp \
  exam_mode.cpp \
  stack_position.cpp \
  storage/file_system.cpp \
  storage/record_name_verifier.cpp \
  storage/record.cpp \
)

ifeq ($(ESCHER_LOG_EVENTS_NAME),1)
ion_device_userland_src += ion/src/shared/layout_events.cpp
endif

tests_src += $(addprefix ion/test/,\
  crc32.cpp\
  events.cpp\
  exam_bytes.cpp\
  exam_mode.cpp\
  keyboard.cpp\
  ring_buffer.cpp \
  storage.cpp \
  utf8_decoder.cpp\
  utf8_helper.cpp\
)

# Export version and patch level
$(call object_for,ion/src/shared/dummy/platform_info.cpp): SFLAGS += -DPATCH_LEVEL=\"$(PATCH_LEVEL)\" -DEPSILON_VERSION=\"$(EPSILON_VERSION)\"

ifdef ION_STORAGE_LOG
SFLAGS += -DION_STORAGE_LOG=1
endif
