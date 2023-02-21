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
  crc32_eat_byte.cpp \
  decompress.cpp \
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
  unicode/code_point.cpp\
  unicode/utf8_decoder.cpp\
  unicode/utf8_helper.cpp\
)

# TODO EMILIE: tidy makefiles: these following should go in ion/src/device/Makefile?

ion_device_bench_src += $(addprefix ion/src/shared/, \
  dummy/assert.cpp \
  console_line.cpp \
  decompress.cpp \
  layout_events.cpp \
  unicode/code_point.cpp\
  unicode/utf8_decoder.cpp \
  unicode/utf8_helper.cpp \
)
ion_device_bench_src += ion/src/external/lz4/lz4.c

ion_device_flasher_src += ion/src/shared/dummy/assert.cpp

ion_device_kernel_src += $(addprefix ion/src/shared/, \
  console_line.cpp:+kernelassert \
  crc32_eat_byte.cpp \
  decompress.cpp \
  events.cpp \
  events_modifier.cpp \
  keyboard.cpp \
  keyboard_queue.cpp \
  layout_events.cpp \
  unicode/utf8_decoder.cpp:+kernelassert \
  unicode/utf8_helper.cpp:+kernelassert \
)
ion_device_kernel_src += ion/src/external/lz4/lz4.c

ion_device_bootloader_src += $(addprefix ion/src/shared/, \
  dummy/assert.cpp \
  decompress.cpp \
)
ion_device_bootloader_src += ion/src/external/lz4/lz4.c

ion_device_userland_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  decompress.cpp:+consoledisplay \
  exam_mode.cpp \
  stack_position.cpp \
  storage/file_system.cpp \
  storage/record_name_verifier.cpp \
  storage/record.cpp \
  unicode/code_point.cpp\
  unicode/utf8_decoder.cpp\
  unicode/utf8_helper.cpp\
)

ifeq ($(ESCHER_LOG_EVENTS_NAME),1)
ion_device_userland_src += ion/src/shared/layout_events.cpp
endif
ion_device_userland_src += ion/src/external/lz4/lz4.c:+consoledisplay

ion_src += ion/src/external/lz4/lz4.c

tests_src += $(addprefix ion/test/,\
  crc32.cpp\
  events.cpp\
  exam_mode.cpp\
  keyboard.cpp\
  persisting_bytes.cpp\
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
