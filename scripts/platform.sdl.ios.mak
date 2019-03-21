TOOLCHAIN ?= ios
EXE = bin

IOS_PLATFORM ?= iphoneos
IOS_MIN_VERSION = 8.0

# Variables below will be autoconfigured

IOS_PLATFORM_VERSION = $(shell xcrun --sdk $(IOS_PLATFORM) --show-sdk-version)
IOS_PLATFORM_BUILD = $(shell xcrun --sdk $(IOS_PLATFORM) --show-sdk-build-version)
IOS_BUILD_MACHINE_OS_BUILD = $(shell sw_vers -buildVersion)
# FIXME: Make the following variables actually automatic
IOS_XCODE_VERSION = "1010"
IOS_XCODE_BUILD = "10B61"
IOS_COMPILER = "com.apple.compilers.llvm.clang.1_0"

BUILD_DIR := $(BUILD_DIR)/$(IOS_PLATFORM)

ifdef ARCH
BUILD_DIR := $(BUILD_DIR)/$(ARCH)
endif
