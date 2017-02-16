SFLAGS += -Iion/include -DKD_CONFIG_H=1
include ion/src/$(PLATFORM)/Makefile

objs += $(addprefix ion/src/shared/, \
  c.o \
  events.o \
  events_$(ION_EVENTS).o \
)

tests += $(addprefix ion/test/,\
  crc32.cpp\
)
