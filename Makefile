CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld.bfd
GDB=arm-none-eabi-gdb
OBJCOPY=arm-none-eabi-objcopy
CFLAGS = -Iarch/stm32f429 -Iexternal/freertos/include -Iexternal -Iexternal/freertos/portable/GCC/ARM_CM4F -Iexternal/newlib/libc/include
#-I/Users/romain/local/arm-none-eabi/include
CFLAGS += -std=c99 -g -Wall # -Os
#CFLAGS += -march=armv7e-m -mcpu=cortex-m4 -mthumb  -mfpu=fpv4-sp-d16
CC=clang
CFLAGS += -target thumbv7em-unknown-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -ffreestanding

objs := boot/crt0.o arch/stm32f429/registers/rcc.o arch/stm32f429/registers/gpio.o external/freertos/tasks.o external/freertos/list.o external/freertos/queue.o external/freertos/portable/GCC/ARM_CM4F/port.o external/freertos/portable/MemMang/heap_1.o external/newlib/libc/string/memset.o external/newlib/libc/string/memcpy.o

default: clean boot.elf

run: boot.elf
	$(GDB) -x gdb_script.gdb boot.elf


test: test.elf
	$(GDB) -x test/gdb_script.gdb test.elf

test.elf: $(objs) test/runner.o
	@$(LD) -T boot/stm32f429_flash.ld $(objs) test/runner.o -o $@

boot.hex: boot.elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O ihex boot.elf boot.hex

boot.bin: boot.elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary boot.elf boot.bin

boot.elf: $(objs) src/freertos_blinky.o
	@echo "LD      $@"
	@$(LD) -T boot/stm32f429_flash.ld $(objs) src/freertos_blinky.o -o $@

%.o: %.c
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "CLEAN"
	@rm -f $(objs) boot.elf boot.bin boot.hex
