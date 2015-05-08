CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld.bfd
GDB=arm-none-eabi-gdb
OBJCOPY=arm-none-eabi-objcopy
CFLAGS = -I. -Iinclude -Iexternal/freertos/include -Iexternal -Iexternal/freertos/portable/GCC/ARM_CM4F -Iexternal/newlib/libc/include
#CFLAGS = -I. -Iexternal/freertos/include -Iexternal -Iexternal/freertos/portable/GCC/ARM_CM4F -Iexternal/newlib/libc/include -Iinclude
#-I/Users/romain/local/arm-none-eabi/include
CFLAGS += -std=c99 -g -Wall
#CFLAGS += -march=armv7e-m -mcpu=cortex-m4 -mthumb  -mfpu=fpv4-sp-d16
CC=clang
CFLAGS += -target thumbv7em-unknown-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -ffreestanding

# Production
#CFLAGS += -Os -fdata-sections -ffunction-sections
#LDFLAGS += --gc-sections

objs := platform/stm32f429/boot/crt0.o
objs += external/freertos/tasks.o external/freertos/list.o external/freertos/queue.o external/freertos/portable/GCC/ARM_CM4F/port.o external/freertos/portable/MemMang/heap_1.o
objs += external/newlib/libc/string/memset.o external/newlib/libc/string/memcpy.o

objs += platform/stm32f429/boot/isr.o platform/stm32f429/registers/gpio.o platform/stm32f429/registers/rcc.o platform/stm32f429/registers/spi.o

objs += platform/stm32f429/init.o platform/stm32f429/init_lcd.o
objs += platform/ili9341/ili9341.o

default: clean boot.elf

run: boot.elf
	$(GDB) -x gdb_script.gdb boot.elf

test: test.elf
	$(GDB) -x test/gdb_script.gdb test.elf

test.elf: $(objs) test/runner.o
	@$(LD) $(LDFLAGS) $(objs) test/runner.o -o $@

boot.hex: boot.elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O ihex boot.elf boot.hex

boot.bin: boot.elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary boot.elf boot.bin

boot.elf: $(objs)
	@echo "LD      $@"
	@$(LD) -T platform/stm32f429/boot/flash.ld $(objs) -o $@

%.o: %.c
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "CLEAN"
	@rm -f $(objs) boot.elf boot.bin boot.hex
