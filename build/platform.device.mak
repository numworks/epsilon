TOOLCHAIN ?= arm-gcc
ifeq ($(COMPILER),llvm)
# Compatibility with old build system
TOOLCHAIN = arm-llvm
endif
USE_LIBA = 1
EXE = elf

%_flash: %.bin
	dfu-util -i 0 -a 0 -s 0x08000000:leave -D $<
