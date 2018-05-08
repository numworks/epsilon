GIT := $(shell command -v git 2> /dev/null)
PATCH_LEVEL = NONE
ifdef GIT
  PATCH_LEVEL = `git rev-parse HEAD | head -c 7`
endif

SFLAGS += -Iion/include -DKD_CONFIG_H=1

include ion/src/$(PLATFORM)/Makefile
include ion/src/shared/tools/Makefile

# We need to work around a GCC bug (concerning versions < 5.1). It is valid in
# C++11 to initialize a character array by providing a string litteral (e.g.
# char test[4]= "ab"; is valid and should initialize test to 'a','b',0,0).
# Older versions of GCC are not conformant so we resort to an initializer list.
initializer_list = $(shell echo $(1) | sed  "s/\(.\)/'\1',/g")0
ion/src/shared/platform_info.o: SFLAGS += -DPATCH_LEVEL="$(call initializer_list,$(PATCH_LEVEL))" -DEPSILON_VERSION="$(call initializer_list,$(EPSILON_VERSION))"

objs += $(addprefix ion/src/shared/, \
  events.o \
  platform_info.o \
  storage.o \
)

tests += $(addprefix ion/test/,\
  crc32.cpp\
  events.cpp\
  keyboard.cpp\
)
