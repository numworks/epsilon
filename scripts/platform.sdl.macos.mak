TOOLCHAIN ?= macos
EXE = bin

ifdef ARCH
BUILD_DIR := $(BUILD_DIR)/$(ARCH)
endif
