
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif

DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')

PREFIX = $(DEVKITPATH)/devkitARM/bin/arm-none-eabi-

CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
LD = $(CXX)
GDB = $(PREFIX)gdb
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size


ARCH = -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS = -g -Wall -O2 -mword-relocations \
         -ffunction-sections \
         $(ARCH)


LIBDIRS	:= $(DEVKITPRO)/libctru

INCLUDE	= $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
          -I$(CURDIR)/$(BUILD)

CFLAGS += $(INCLUDE) -DARM11 -D_3DS -D__3DS__

CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS = -g $(ARCH)
LDFLAGS = -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS = -lctru -lm


# Always generate debug information
SFLAGS += -ggdb3

# Put data/code symbols in their own subsection
# This allows the linker script to precisely place a given symbol
SFLAGS += -fdata-sections -ffunction-sections

ifeq ($(DEBUG),1)
LTO ?= 0
else
LTO ?= 1
endif

ifeq ($(LTO),1)
# Use link-time optimization if LTO=1
SFLAGS += -flto
endif

# Get rid of unused symbols. This is also useful even if LTO=1.
LDFLAGS += -Wl,--gc-sections

LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

LDFLAGS += $(SFLAGS) $(LIBPATHS) $(LIBS) -lgcc

# To debug linker scripts, add the following line
# LDFLAGS += -Wl,-M
