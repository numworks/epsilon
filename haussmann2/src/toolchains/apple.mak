ifeq ($(APPLE_PLATFORM),ios)
APPLE_SDK = iphoneos
APPLE_PLATFORM_MIN_VERSION_KEYWORD = iphoneos
else ifeq ($(APPLE_PLATFORM),ios-simulator)
APPLE_SDK = iphonesimulator
APPLE_PLATFORM_MIN_VERSION_KEYWORD = ios-simulator
else ifeq ($(APPLE_PLATFORM),macos)
APPLE_SDK = macosx
APPLE_PLATFORM_MIN_VERSION_KEYWORD = macosx
else
$(error Unrecognized APPLE_PLATFORM)
endif

IOS_PLATFORM_VERSION := $(shell xcrun --sdk $(APPLE_SDK) --show-sdk-version)
IOS_PLATFORM_BUILD := $(shell xcrun --sdk $(APPLE_SDK) --show-sdk-build-version)
IOS_BUILD_MACHINE_OS_BUILD := $(shell sw_vers -buildVersion)
# FIXME: Make the following variables actually automatic
IOS_XCODE_VERSION = "1010"
IOS_XCODE_BUILD = "10B61"
IOS_COMPILER = "com.apple.compilers.llvm.clang.1_0"

CC := $(shell xcrun --sdk $(APPLE_SDK) --find clang)
CXX := $(shell xcrun --sdk $(APPLE_SDK) --find clang++)
LD := $(shell xcrun --sdk $(APPLE_SDK) --find clang++)
ACTOOL = $(shell xcrun --sdk $(APPLE_SDK) --find actool)
IBTOOL = $(shell xcrun --sdk $(APPLE_SDK) --find ibtool)
LIPO = $(shell xcrun --sdk $(APPLE_SDK) --find lipo)

EXECUTABLE_EXTENSION := bin

SYSROOT := $(shell xcrun --sdk $(APPLE_SDK) --show-sdk-path)
export SDKROOT := $(shell xcrun --show-sdk-path)

# TODO SFLAGS += -arch $(ARCH)
SFLAGS += -isysroot $(SYSROOT)
SFLAGS += -fPIC
SFLAGS += -m$(APPLE_PLATFORM_MIN_VERSION_KEYWORD)-version-min=$(APPLE_PLATFORM_MIN_VERSION)
