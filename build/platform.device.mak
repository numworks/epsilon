TOOLCHAIN ?= arm-gcc
ifeq ($(COMPILER),llvm)
# Compatibility with old build system
TOOLCHAIN = arm-llvm
endif
USE_LIBA = 1
EXE = elf

.PHONY: %_run
%_run: %.$(EXE)
	$(GDB) -x gdb_script.gdb $<

.PHONY: %_flash
%_flash: %.bin
	dfu-util -i 0 -a 0 -s 0x08000000:leave -D $<
