TOOLCHAIN = android

EPSILON_TELEMETRY ?= 1

ARCHS = armeabi-v7a arm64-v8a x86 x86_64

ifdef ARCH
EXE = so
BUILD_DIR := $(BUILD_DIR)/$(ARCH)
endif
