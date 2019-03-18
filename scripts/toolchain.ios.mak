CC = clang
CXX = clang++
LD = clang++

#ARCH = x86_64
#SDK = iphonesimulator
# ARCH = arm64
# SDK = iphoneos

SYSROOT = $(shell xcrun --sdk $(IOS_PLATFORM) --show-sdk-path)

SFLAGS += -arch $(ARCH)
SFLAGS += -isysroot $(SYSROOT)
SFLAGS += -fPIC
SFLAGS += -miphoneos-version-min=$(IOS_MIN_VERSION)
#SFLAGS += -mios-simulator-version-min=$(IOS_MIN_VERSION)

LDFLAGS += -arch $(ARCH)
LDFLAGS += -isysroot $(SYSROOT)
LDFLAGS += -miphoneos-version-min=$(IOS_MIN_VERSION)
