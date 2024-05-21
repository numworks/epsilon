TOOLCHAIN ?= apple

APPLE_PLATFORM := macos
APPLE_PLATFORM_MIN_VERSION := 10.10

ifeq ($(DEBUG),0)
ARCHS = arm64 x86_64
else
ARCHS ?= $(shell uname -m)
endif
