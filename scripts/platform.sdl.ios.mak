TOOLCHAIN ?= ios
EXE = bin

IOS_PLATFORM ?= iphoneos
IOS_MIN_VERSION = 8.0

# Variables below will e autoconfigured
#
IOS_PLATFORM_VERSION = $(shell xcrun --sdk $(IOS_PLATFORM) --show-sdk-version)
IOS_PLATFORM_BUILD = $(shell xcrun --sdk $(IOS_PLATFORM) --show-sdk-build-version)

BUILD_DIR := $(BUILD_DIR)/$(IOS_PLATFORM)

ifdef ARCH
BUILD_DIR := $(BUILD_DIR)/$(ARCH)
endif
