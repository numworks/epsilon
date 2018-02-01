include build/config.mak

VERSION ?= 1.3.0

ifndef USE_LIBA
  $(error platform.mak should define USE_LIBA)
endif
ifndef EXE
  $(error platform.mak should define EXE, the extension for executables)
endif

HOSTCC = gcc
HOSTCXX = g++

# Flags - Optimizations
SFLAGS += $(OPTIM_SFLAGS)

# Flags - Header search path
SFLAGS += -Ilib -I.

# Flags - Building options
SFLAGS += -Wall

# Flags - Header dependency tracking
SFLAGS += -MD -MP

# Language-specific flags
CFLAGS = -std=c99
CXXFLAGS = -std=c++11 -fno-exceptions -fno-rtti -fno-threadsafe-statics

products :=

default: app.$(EXE)
run: app_run

.PHONY: info
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
	@echo "EPSILON_APPS = $(EPSILON_APPS)"
	@echo "EPSILON_I18N_LANGUAGES = $(EPSILON_I18N_LANGUAGES)"
	@echo "==============================="

# Each sub-Makefile can either add objects to the $(objs) variable or define a
# new executable target. The $(objs) variable lists the objects that will be
# linked to every executable being generated. Each Makefile is also responsible
# for keeping the $(product) variable updated. This variable lists all files
# that could be generated during the build and that needs to be cleaned up
# afterwards.

# Library Makefiles
ifeq ($(USE_LIBA),0)
include liba/Makefile.bridge
else
SFLAGS += -ffreestanding -nostdinc -nostdlib
include liba/Makefile
include libaxx/Makefile
endif
include ion/Makefile
include kandinsky/Makefile
include poincare/Makefile
include python/Makefile
include escher/Makefile
# Executable Makefiles
include apps/Makefile
include build/struct_layout/Makefile
include quiz/Makefile # Quiz needs to be included at the end

products += $(objs)

all_objs = $(filter %.o, $(products))
dependencies = $(all_objs:.o=.d)
-include $(dependencies)
products += $(dependencies)

$(all_objs): $(generated_headers)

.SECONDARY: $(objs)
%.$(EXE): $(objs)
	@echo "LD      $@"
	@$(LD) $^ $(LDFLAGS) -o $@

%.o: %.c
	@echo "CC      $@"
	@$(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo "AS      $@"
	@$(CC) $(SFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "CXX     $@"
	@$(CXX) $(SFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "CLEAN"
	@rm -f $(products)

-include build/targets.$(PLATFORM).mak
