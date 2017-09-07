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
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press the RESET button the back of your device."
	@until dfu-util -l | grep "Internal Flash" > /dev/null 2>&1; do sleep 1;done
	@echo "DFU     $@"
	@dfu-util -i 0 -a 0 -s 0x08000000:leave -D $<
