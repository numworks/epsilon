PLATFORM ?= device
DEBUG ?= 1

include Makefile.$(PLATFORM)
ifndef USE_LIBA
  $(error Makefile.PLATFORM should define USE_LIBA)
endif

# Flags - Header search path
SFLAGS += -Ilib -I.

# Flags - Building options
SFLAGS += -Wall

# Flags - Optimizations
ifeq ($(DEBUG),1)
SFLAGS += -ggdb3 -DDEBUG=1 -O0
else
SFLAGS += -Os -fdata-sections -ffunction-sections
#LDFLAGS += --gc-sections
#FIXME: --gc-sections doesn't seem to be working
endif

# Language-specific flags
CFLAGS = -std=c99
CXXFLAGS = -std=c++11 -fno-exceptions -fno-rtti #-fno-unwind-tables

products := boot.elf boot.hex boot.bin test.elf test.hex test.bin

#lib/private/mem5.o: CFLAGS += -w

ifeq ($(VERBOSE),1)
default: info clean app_size app_memory_map
else
default: app.elf
endif
run: app_run

info:
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

ifeq ($(USE_LIBA),0)
#LDFLAGS += -lc -lc++ -lcrt1.o
else
SFLAGS += -ffreestanding -nostdinc -nostdlib
include liba/Makefile
include libaxx/Makefile
endif
include ion/Makefile
include kandinsky/Makefile
include poincare/Makefile
include app/Makefile
include quiz/Makefile # Quiz should be included at the end

%.elf: $(objs)
	@echo "LD      $@"
	@$(LD) $^ $(LDFLAGS) -o $@

.PHONY: %_size
%_size: %.elf
	@echo "========= BUILD OUTPUT ========"
	@echo "File:  $<"
	@$(SIZE) $< | tail -n 1 | awk '{print "Code:  " $$1 " bytes";print "Data:  " $$2 " bytes"; print "Total: " int(($$1+$$2)/1024) " kB (" $$1 + $$2 " bytes)";}'
	@echo "==============================="

.PHONY: %_run
%_run: %.elf
	$(GDB) -x gdb_script.gdb $<

%.hex: %.elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@

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
