PLATFORM ?= device
DEBUG ?= 1

.PHONY: prebuild default postbuild
default: prebuild boot.elf postbuild
prebuild:
postbuild:

.PHONY: tests
test: prebuild test.elf postbuild

include Makefile.$(PLATFORM)
ifndef USE_LIBA
  $(error Makefile.PLATFORM should define USE_LIBA)
endif
SFLAGS += -DPLATFORM=$(PLATFORM)

# Flags - Header search path
SFLAGS += -Ilib -I.

# Flags - Building options
SFLAGS += -Wall

# Flags - Optimizations
ifeq ($(DEBUG),1)
SFLAGS += -g -DDEBUG=1
else
SFLAGS += -Os -fdata-sections -ffunction-sections
#LDFLAGS += --gc-sections
#FIXME: --gc-sections doesn't seem to be working
endif

# Language-specific flags
CFLAGS = -std=c99
CXXFLAGS = -std=c++11 -fno-exceptions -fno-unwind-tables -fno-rtti

products := boot.elf boot.hex boot.bin test.elf test.hex test.bin

#objs += external/freertos/tasks.o external/freertos/list.o external/freertos/queue.o external/freertos/portable/GCC/ARM_CM4F/port.o external/freertos/portable/MemMang/heap_1.o
#objs += $(addprefix external/newlib/libc/, string/memset.o string/memcpy.o string/strlen.o)

lib/private/mem5.o: CFLAGS += -w

products += src/hello.o
boot.elf: src/hello.o

ifeq ($(VERBOSE),1)
.PHONY: toolchain_info output_size
prebuild: toolchain_info
toolchain_info:
	@echo "========= BUILD SETTINGS ======"
	@echo "DEBUG = $(DEBUG)"
	@echo "PLATFORM = $(PLATFORM)"
	@echo "CC = $(CC)"
	@echo "CXX = $(CXX)"
	@echo "LD = $(LD)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "SFLAGS = $(SFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "==============================="
postbuild: output_size
output_size: boot.elf
	@echo "========= BUILD OUTPUT ========"
	@echo "File:  $<"
	@$(SIZE) $< | tail -n 1 | awk '{print "Code:  " $$1 " bytes";print "Data:  " $$2 " bytes"; print "Total: " int(($$1+$$2)/1024) " kB (" $$1 + $$2 " bytes)";}'
	@echo "==============================="
endif

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
include app/Makefile

# Quiz should be included at the end
include quiz/Makefile

run: boot.elf
	$(GDB) -x gdb_script.gdb boot.elf

%.hex: %.elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@

%.elf: $(objs)
	@echo "LD      $@"
	@$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c
	@echo "CC      $@"
	@$(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "CXX     $@"
	@$(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "CLEAN"
	@rm -f $(objs) $(test_objs) $(products)
