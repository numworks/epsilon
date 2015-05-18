CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld.bfd
GDB=arm-none-eabi-gdb
OBJCOPY=arm-none-eabi-objcopy
CFLAGS = -Ilib -I. -Iinclude -Iexternal/freertos/include -Iexternal -Iexternal/freertos/portable/GCC/ARM_CM4F -Iexternal/newlib/libc/include
CFLAGS += -DHAVE_CONFIG_H=1 -DPIXMAN_NO_TLS=1 -Wno-unused-const-variable
#CFLAGS += -fshort-double # Use the FPU even for doubles
#CFLAGS = -I. -Iexternal/freertos/include -Iexternal -Iexternal/freertos/portable/GCC/ARM_CM4F -Iexternal/newlib/libc/include -Iinclude
#-I/Users/romain/local/arm-none-eabi/include
CFLAGS += -std=c99 -Wall
#CFLAGS += -march=armv7e-m -mcpu=cortex-m4 -mthumb  -mfpu=fpv4-sp-d16
CC=clang
CFLAGS += -target thumbv7em-unknown-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -ffreestanding

CXX=clang++
CXXFLAGS=-target thumbv7em-unknown-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -ffreestanding
CXXFLAGS += -fno-exceptions -fno-unwind-tables -fno-rtti -nostdlib

# Production
CFLAGS += -g
#CFLAGS += -Os -fdata-sections -ffunction-sections
#LDFLAGS += --gc-sections

products := boot.elf boot.hex boot.bin

objs += external/freertos/tasks.o external/freertos/list.o external/freertos/queue.o external/freertos/portable/GCC/ARM_CM4F/port.o external/freertos/portable/MemMang/heap_1.o
objs += external/newlib/libc/string/memset.o external/newlib/libc/string/memcpy.o

objs += lib/assert.o

objs += math/expression.o


default: clean boot.elf

include platform/Makefile
include kandinsky/Makefile

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

%.o: %.cpp
	@echo "CXX     $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "CLEAN"
	@rm -f $(objs) $(products)
