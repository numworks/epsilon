TOOLCHAIN = apple
EXE = bin

APPLE_PLATFORM = macos
APPLE_PLATFORM_MIN_VERSION = 10.10
EPSILON_TELEMETRY ?= 0

ARCHS = x86_64

ifdef ARCH
BUILD_DIR := $(BUILD_DIR)/$(ARCH)
else
HANDY_TARGETS_EXTENSIONS = app
endif

# Reload the simulator
reload: default
	$(Q) pgrep Epsilon && pkill -USR1 Epsilon || echo "No Epsilon executable running"
	$(Q) open output/debug/simulator/macos/epsilon.app --args --load-state-file ~/reload.nws
