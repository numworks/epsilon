TOOLCHAIN ?= arm-gcc
USE_LIBA = 1
EXE = elf

python/port/port.o: CXXFLAGS += -DMP_PORT_USE_STACK_SYMBOLS=1
