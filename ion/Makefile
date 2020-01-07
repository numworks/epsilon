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

include ion/src/$(PLATFORM)/Makefile
-include ion/test/$(PLATFORM)/Makefile
include ion/src/shared/tools/Makefile

ion_console_display_src += ion/src/shared/console_display.cpp

# We need to work around a GCC bug (concerning versions < 5.1). It is valid in
# C++11 to initialize a character array by providing a string litteral (e.g.
# char test[4]= "ab"; is valid and should initialize test to 'a','b',0,0).
# Older versions of GCC are not conformant so we resort to an initializer list.
initializer_list = $(shell echo $(1) | sed  "s/\(.\)/'\1',/g")0
$(call object_for,ion/src/shared/platform_info.cpp): SFLAGS += -DPATCH_LEVEL="$(call initializer_list,$(PATCH_LEVEL))" -DEPSILON_VERSION="$(call initializer_list,$(EPSILON_VERSION))"

ion_src += $(addprefix ion/src/shared/, \
  console_line.cpp \
  crc32_eat_byte.cpp \
  decompress.cpp \
  events.cpp \
  platform_info.cpp \
  storage.cpp \
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

# Configure variants
ion_all_src = $(ion_src)
ion_all_src += $(ion_simulator_sdl_src) $(ion_simulator_headless_src)
ion_all_src += $(ion_device_dfu_relocated_src) $(ion_device_dfu_xip_src)
ion_all_src += $(ion_console_display_src) $(ion_console_stdio_src) $(ion_console_uart_src)

ion_default_src = $(ion_src) $(ion_simulator_sdl_src) $(ion_device_dfu_relocated_src) $(ion_console_stdio_src)
ion_console_on_screen_src = $(ion_src) $(ion_simulator_sdl_src) $(ion_device_dfu_relocated_src) $(ion_console_display_src)
ion_xip_src = $(ion_src) $(ion_simulator_sdl_src) $(ion_device_dfu_xip_src) $(ion_console_uart_src)
ion_headless_src = $(ion_src) $(ion_simulator_headless_src) $(ion_device_dfu_relocated_src) $(ion_console_stdio_src)
