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

include ion/src/$(PLATFORM)/Makefile
-include ion/test/$(PLATFORM)/Makefile
include ion/src/shared/tools/Makefile

# We need to work around a GCC bug (concerning versions < 5.1). It is valid in
# C++11 to initialize a character array by providing a string literal (e.g.
# char test[4]= "ab"; is valid and should initialize test to 'a','b',0,0).
# Older versions of GCC are not conformant so we resort to an initializer list.
initializer_list = $(shell echo $(1) | sed  "s/\(.\)/'\1',/g")0
$(call object_for,ion/src/simulator/platform_info.cpp ion/src/device/n0100/platform_info.cpp ion/src/device/n0110/platform_info.cpp ion/src/device/bootloader/platform_info.cpp ion/src/simulator/shared/platform_info.cpp): SFLAGS += -DPATCH_LEVEL="$(call initializer_list,$(PATCH_LEVEL))" -DEPSILON_VERSION="$(call initializer_list,$(EPSILON_VERSION))" -DOMEGA_VERSION="$(call initializer_list,$(OMEGA_VERSION))" -DUPSILON_VERSION="$(call initializer_list,$(UPSILON_VERSION))" -DOMEGA_USERNAME="$(call initializer_list,$(OMEGA_USERNAME))" 

ion_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  crc32_eat_byte.cpp \
  decompress.cpp \
  events.cpp \
  events_keyboard.cpp \
  events_modifier.cpp \
  rtc.cpp \
  stack_position.cpp \
  storage.cpp \
  internal_storage.cpp \
  unicode/utf8_decoder.cpp\
  unicode/utf8_helper.cpp\
)

ion_src += ion/src/external/lz4/lz4.c

tests_src += $(addprefix ion/test/,\
  crc32.cpp\
  events.cpp\
  keyboard.cpp\
  storage.cpp\
  utf8_decoder.cpp\
  utf8_helper.cpp\
)

ifdef ION_STORAGE_LOG
SFLAGS += -DION_STORAGE_LOG=1
endif
