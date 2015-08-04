PLATFORM ?= stm32f429
#PLATFORM=simulator
include Makefile.$(PLATFORM)
ifndef USE_LIBA
  $(error Makefile.PLATFORM should define USE_LIBA)
endif
SFLAGS += -DUSE_LIBA=$(USE_LIBA)

# Flags - Header search path
SFLAGS += -Ilib -I.

# Flags - Building options
SFLAGS += -Wall

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
CXXFLAGS = -std=c++11 -fno-exceptions -fno-unwind-tables -fno-rtti

products := boot.elf boot.hex boot.bin

#objs += external/freertos/tasks.o external/freertos/list.o external/freertos/queue.o external/freertos/portable/GCC/ARM_CM4F/port.o external/freertos/portable/MemMang/heap_1.o
#objs += $(addprefix external/newlib/libc/, string/memset.o string/memcpy.o string/strlen.o)

lib/private/mem5.o: CFLAGS += -w

#objs += src/hello.o

.PHONY: default info
default: info clean boot.elf

ifeq ($(VERBOSE),1)
info:
	@echo "========= BUILD SETTINGS ======"
	@echo "PLATFORM = $(PLATFORM)"
	@echo "CC = $(CC)"
	@echo "CXX = $(CXX)"
	@echo "LD = $(LD)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "SFLAGS = $(SFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "==============================="
else
info:
endif

include boot/Makefile
ifeq ($(USE_LIBA),0)
LDFLAGS += -lc -lc++ -lcrt1.o
else
SFLAGS += -ffreestanding -nostdinc -nostdlib
include liba/Makefile
include libaxx/Makefile
endif
include ion/Makefile
include kandinsky/Makefile
include poincare/Makefile

objs += src/hello.o

size: boot.elf
	@echo "========= BUILD OUTPUT ========"
	@echo "File:  $<"
	@arm-none-eabi-size $< | tail -n 1 | awk '{print "Code:  " $$1 " bytes";print "Data:  " $$2 " bytes"; print "Total: " $$1 + $$2 " bytes";}'
	@echo "==============================="

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
	@$(LD) $(LDFLAGS) $(objs) -o $@

%.o: %.c
	@echo "CC      $@"
	@$(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "CXX     $@"
	@$(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "CLEAN"
	@rm -f $(objs) $(products)
