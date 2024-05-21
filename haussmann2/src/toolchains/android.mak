ANDROID_HOME ?= /usr/local/android
NDK_BUNDLE_VERSION ?= 22.1.7171670
NDK_PATH ?= $(ANDROID_HOME)/ndk/$(NDK_BUNDLE_VERSION)

ifeq ($(HOST),macos)
NDK_HOST_TAG ?= darwin-x86_64
else ifeq ($(HOST),linux)
NDK_HOST_TAG ?= linux-x86_64
endif

NDK_TOOLCHAIN_PATH = $(NDK_PATH)/toolchains/llvm/prebuilt/$(NDK_HOST_TAG)/bin

# No 64 bit device has ever shipped with an API level < 21. Consequently, there
# is no toolchain for those archs on those API levels. Let's enforce NDK_VERSION
# at 21 for these archs, and 16 for the others.

$(OUTPUT_DIRECTORY)/armeabi-v7a/%: NDK_TARGET := armv7a-linux-androideabi
$(OUTPUT_DIRECTORY)/armeabi-v7a/%: NDK_VERSION := 16

$(OUTPUT_DIRECTORY)/arm64-v8a/%: NDK_TARGET := aarch64-linux-android
$(OUTPUT_DIRECTORY)/arm64-v8a/%: NDK_VERSION := 21

$(OUTPUT_DIRECTORY)/x86/%: NDK_TARGET := i686-linux-android
$(OUTPUT_DIRECTORY)/x86/%: NDK_VERSION := 16

$(OUTPUT_DIRECTORY)/x86_64/%: NDK_TARGET := x86_64-linux-android
$(OUTPUT_DIRECTORY)/x86_64/%: NDK_VERSION := 21

CC = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang
CXX = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang++
AR = $(NDK_TOOLCHAIN_PATH)/$(subst v7a,,$(NDK_TARGET))-ar
LD = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang++

EXECUTABLE_EXTENSION := so

SFLAGS += -fPIC
# If MICROPY_NLR_SETJMP is 0, the MicroPython NLR is done by
# python/src/py/nlrthumb.c and creates code containing relocations, which is not
# accepted by Android.
SFLAGS += -DMICROPY_NLR_SETJMP=1

LDFLAGS += \
  -shared \
  -static-libstdc++
# Warn on missing symbols when linking the dynamic library
LDFLAGS += -Wl,-z,defs
