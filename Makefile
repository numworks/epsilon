TOOLCHAIN=arm-none-eabi
COMPILER=llvm

ifeq ($(COMPILER),llvm)
CC=clang
CXX=clang++
else
CC=$(TOOLCHAIN)-gcc
CXX=$(TOOLCHAIN)-g++
endif

LD=$(TOOLCHAIN)-ld.bfd
GDB=$(TOOLCHAIN)-gdb
OBJCOPY=$(TOOLCHAIN)-objcopy

# Compiler flags
# Note: We're using CFLAGS, CXXFLAGS, and SFLAGS. SFLAGS are shared flags for both C and C++

# Flags - Arch
ifeq ($(COMPILER),llvm)
  SFLAGS += -target thumbv7em-unknown-eabi
else
  SFLAGS += -mthumb -march=armv7e-m -mfloat-abi=softfp
endif
SFLAGS += -mcpu=cortex-m4 -mfpu=fpv4-sp-d16

# Flags - Header search path
SFLAGS += -Ilib -I. -Iinclude -Iexternal/freertos/include -Iexternal -Iexternal/freertos/portable/GCC/ARM_CM4F -Iexternal/newlib/libc/include

# Flags - Building options
SFLAGS += -Wall -ffreestanding

# Flags - Optimizations
ifeq ($(PRODUCTION),1)
#echo "*** PRODUCTION BUILD ***"
SFLAGS += -Os -fdata-sections -ffunction-sections
LDFLAGS += --gc-sections
else
SFLAGS += -g
endif

# Language-specific flags
CFLAGS = -std=c99
CXXFLAGS = -std=c++11 -fno-exceptions -fno-unwind-tables -fno-rtti -nostdlib

products := boot.elf boot.hex boot.bin

objs += external/freertos/tasks.o external/freertos/list.o external/freertos/queue.o external/freertos/portable/GCC/ARM_CM4F/port.o external/freertos/portable/MemMang/heap_1.o
objs += $(addprefix external/newlib/libc/, string/memset.o string/memcpy.o string/strlen.o)

lib/private/mem5.o: CFLAGS += -w
objs += lib/assert.o lib/errno.o lib/private/mem5.o lib/cxx_new.o

objs += src/hello.o


default: clean boot.elf

include platform/Makefile
include kandinsky/Makefile
include poincare/Makefile

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
	@$(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "CXX     $@"
	@$(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "CLEAN"
	@rm -f $(objs) $(products)
