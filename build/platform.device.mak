TOOLCHAIN ?= arm-gcc
ifeq ($(COMPILER),llvm)
# Compatibility with old build system
TOOLCHAIN = arm-llvm
endif
USE_LIBA = 1
EXE = elf
