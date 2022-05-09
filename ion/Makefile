GIT := $(shell command -v git 2> /dev/null)
PATCH_LEVEL = NONE
ifdef GIT
  PATCH_LEVEL = $(shell git rev-parse HEAD | head -c 7)
endif

SFLAGS += -Iion/include -DKD_CONFIG_H=1

ifndef ION_KEYBOARD_LAYOUT
  $(error platform.mak should define ION_KEYBOARD_LAYOUT)
endif
SFLAGS += -Iion/include/ion/keyboard/$(ION_KEYBOARD_LAYOUT)
SFLAGS += -Iion/include/ion/keyboard/
ion_src += ion/src/shared/keyboard/$(ION_KEYBOARD_LAYOUT)/layout_events.cpp

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
  keyboard.cpp \
  keyboard_queue.cpp \
  stack_position.cpp \
  storage/container.cpp \
  storage/record_name_verifier.cpp \
  storage/record.cpp \
  unicode/code_point.cpp\
  unicode/utf8_decoder.cpp\
  unicode/utf8_helper.cpp\
)

# TODO EMILIE: tidy makefiles: these following should go in ion/src/device/Makefile?

ion_device_bench_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  decompress.cpp \
  unicode/code_point.cpp\
  unicode/utf8_decoder.cpp \
  unicode/utf8_helper.cpp \
  )

ion_device_bench_src += ion/src/external/lz4/lz4.c

ion_device_kernel_src += $(addprefix ion/src/shared/, \
  crc32_eat_byte.cpp \
  decompress.cpp \
  events.cpp \
  events_modifier.cpp \
  keyboard.cpp \
  keyboard_queue.cpp \
)

ion_device_kernel_src += ion/src/external/lz4/lz4.c

ion_device_bootloader_src += $(addprefix ion/src/shared/, \
  decompress.cpp \
)
ion_device_bootloader_src += ion/src/external/lz4/lz4.c

ion_device_userland_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  decompress.cpp:+consoledisplay \
  stack_position.cpp \
  storage/container.cpp \
  storage/record_name_verifier.cpp \
  storage/record.cpp \
  unicode/code_point.cpp\
  unicode/utf8_decoder.cpp\
  unicode/utf8_helper.cpp\
)

ion_device_bench_src += ion/src/shared/keyboard/$(ION_KEYBOARD_LAYOUT)/layout_events.cpp
ion_device_kernel_src += ion/src/shared/keyboard/$(ION_KEYBOARD_LAYOUT)/layout_events.cpp
ifeq ($(ESCHER_LOG_EVENTS_NAME),1)
ion_device_userland_src += ion/src/shared/keyboard/$(ION_KEYBOARD_LAYOUT)/layout_events.cpp
endif
ion_device_userland_src += ion/src/external/lz4/lz4.c:+consoledisplay

ion_src += ion/src/external/lz4/lz4.c

tests_src += $(addprefix ion/test/,\
  crc32.cpp\
  events.cpp\
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
