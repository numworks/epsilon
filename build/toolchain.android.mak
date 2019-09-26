NDK_PATH ?= /usr/local/android/ndk-bundle

ifeq ($(HOST),macos)
NDK_HOST_TAG ?= darwin-x86_64
else ifeq ($(HOST),linux)
NDK_HOST_TAG ?= linux-x86_64
endif

NDK_TOOLCHAIN_PATH = $(NDK_PATH)/toolchains/llvm/prebuilt/$(NDK_HOST_TAG)/bin

ifeq ($(NDK_ABI),armeabi-v7a)
  NDK_TARGET = armv7a-linux-androideabi
else ifeq ($(NDK_ABI),arm64-v8a)
  NDK_TARGET = aarch64-linux-android
else ifeq ($(NDK_ABI),x86)
  NDK_TARGET = i686-linux-android
else ifeq ($(NDK_ABI),x86_64)
  NDK_TARGET = x86_64-linux-android
endif

ifdef NDK_TARGET

NDK_VERSION ?= 21

CC = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang
CXX = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang++
LD = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang++

SFLAGS += -fPIC

LDFLAGS += -shared
LDFLAGS += -static-libstdc++
LDFLAGS += -Wl,-z,defs # Warn on missing symbols when linking the dynamic library

else

CC = false
CXX = false
LD = false

endif
