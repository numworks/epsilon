GIT := $(shell command -v git 2> /dev/null)
PATCH_LEVEL = NONE
ifdef GIT
  PATCH_LEVEL = `git rev-parse --short HEAD`
endif

SFLAGS += -Iion/include -DKD_CONFIG_H=1

include ion/src/$(PLATFORM)/Makefile
include ion/src/shared/tools/Makefile

ion/src/shared/software_version.o: SFLAGS += -DPATCH_LEVEL=$(PATCH_LEVEL) -DVERSION=$(VERSION)

objs += $(addprefix ion/src/shared/, \
  events.o \
  software_version.o \
)

tests += $(addprefix ion/test/,\
  crc32.cpp\
  events.cpp\
  keyboard.cpp\
)
