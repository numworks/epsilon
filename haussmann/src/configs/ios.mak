TOOLCHAIN ?= apple

APPLE_PLATFORM ?= ios
APPLE_PLATFORM_MIN_VERSION := 8.0

ifeq ($(APPLE_PLATFORM),ios)
ARCHS ?= arm64
else ifeq ($(APPLE_PLATFORM),ios-simulator)
ARCHS ?= x86_64
endif
