TOOLCHAIN ?= android
EXE = so

ifdef NDK_ABI
BUILD_DIR := $(BUILD_DIR)/$(NDK_ABI)
endif
