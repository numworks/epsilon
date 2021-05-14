TOOLCHAIN = android
EXE = so

EPSILON_TELEMETRY ?= 0

ARCHS = armeabi-v7a arm64-v8a x86 x86_64

ifdef ARCH
BUILD_DIR := $(BUILD_DIR)/$(ARCH)
else
HANDY_TARGETS_EXTENSIONS = apk
endif
