TOOLCHAIN ?= apple

APPLE_PLATFORM := macos
APPLE_PLATFORM_MIN_VERSION := 10.10

ifeq ($(DEBUG),0)
ARCHS ?= arm64 x86_64
else
# Use the "if" construct instead of ?=. If ?= was used, ARCHS would be defined
# as a recursive variable instead of a simply expanded one, and a call to
# "shell" in a recursive variable can be very costly.
ARCHS := $(if $(ARCHS),$(ARCHS),$(shell uname -m))
endif
