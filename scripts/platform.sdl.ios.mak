TOOLCHAIN ?= ios
EXE = bin

SDK ?= iphonesimulator

BUILD_DIR := $(BUILD_DIR)/$(SDK)

ifdef ARCH
BUILD_DIR := $(BUILD_DIR)/$(ARCH)
endif
