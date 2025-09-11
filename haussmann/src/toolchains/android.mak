ANDROID_HOME ?= /usr/local/android
NDK_BUNDLE_VERSION ?= 28.2.13676358
NDK_PATH ?= $(ANDROID_HOME)/ndk/$(NDK_BUNDLE_VERSION)

ifeq ($(HOST),macos)
NDK_HOST_TAG ?= darwin-x86_64
else ifeq ($(HOST),linux)
NDK_HOST_TAG ?= linux-x86_64
endif

NDK_TOOLCHAIN_PATH = $(NDK_PATH)/toolchains/llvm/prebuilt/$(NDK_HOST_TAG)/bin

# The lowest Android API level supported by the NDK can be read in
# $ANDROID_HOME/ndk/$NDK_BUNDLE_VERSION/meta/platforms.json.
# The minSdkVersion set in gradle.build should be equal or more than this.
NDK_MIN_API_LEVEL := 21

$(OUTPUT_DIRECTORY)/armeabi-v7a/%: NDK_TARGET := armv7a-linux-androideabi
$(OUTPUT_DIRECTORY)/arm64-v8a/%: NDK_TARGET := aarch64-linux-android
$(OUTPUT_DIRECTORY)/x86/%: NDK_TARGET := i686-linux-android
$(OUTPUT_DIRECTORY)/x86_64/%: NDK_TARGET := x86_64-linux-android

CC = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_MIN_API_LEVEL)-clang
CXX = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_MIN_API_LEVEL)-clang++
AR = $(NDK_TOOLCHAIN_PATH)/llvm-ar
LD = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_MIN_API_LEVEL)-clang++

EXECUTABLE_EXTENSION := so

COMPILER_FAMILY := clang

SFLAGS += -fPIC

LDFLAGS += \
  -shared \
  -static-libstdc++
# Warn on missing symbols when linking the dynamic library
LDFLAGS += -Wl,-z,defs
