TOOLCHAIN ?= apple

APPLE_PLATFORM := macos
APPLE_PLATFORM_MIN_VERSION := 10.10

# Disable since sharing binaries on macos is difficult anyway
# ifeq ($(DEBUG),0)
# ARCHS ?= arm64 x86_64
# endif

# Use the "if" construct instead of ?=. If ?= was used, ARCHS would be defined
# as a recursive variable instead of a simply expanded one, and a call to
# "shell" in a recursive variable can be very costly.
ARCHS := $(if $(ARCHS),$(ARCHS),$(shell uname -m))
