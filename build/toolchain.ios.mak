ifdef ARCH

CC = $(shell xcrun --sdk $(IOS_PLATFORM) --find clang)
CXX = $(shell xcrun --sdk $(IOS_PLATFORM) --find clang++)
LD = $(shell xcrun --sdk $(IOS_PLATFORM) --find clang++)

SYSROOT = $(shell xcrun --sdk $(IOS_PLATFORM) --show-sdk-path)

SFLAGS += -arch $(ARCH)
SFLAGS += -isysroot $(SYSROOT)
SFLAGS += -fPIC
SFLAGS += -miphoneos-version-min=$(IOS_MIN_VERSION)

LDFLAGS += -arch $(ARCH)
LDFLAGS += -isysroot $(SYSROOT)
LDFLAGS += -miphoneos-version-min=$(IOS_MIN_VERSION)

else

CC = false
CXX = false
LD = false

endif

ACTOOL = $(shell xcrun --sdk $(IOS_PLATFORM) --find actool)
IBTOOL = $(shell xcrun --sdk $(IOS_PLATFORM) --find ibtool)
LIPO = $(shell xcrun --sdk $(IOS_PLATFORM) --find lipo)
