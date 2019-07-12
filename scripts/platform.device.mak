MODEL ?= n0110
USE_LIBA = 1
EXE = elf

BUILD_DIR = build/$(MODE)/$(PLATFORM)/$(MODEL)

$(BUILD_DIR)/python/port/port.o: CXXFLAGS += -DMP_PORT_USE_STACK_SYMBOLS=1

include scripts/platform.device.$(MODEL).mak
