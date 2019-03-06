NDK_PATH ?= /usr/local/android/ndk-bundle
NDK_HOST_TAG ?= darwin-x86_64

NDK_TOOLCHAIN_PATH = $(NDK_PATH)/toolchains/llvm/prebuilt/$(NDK_HOST_TAG)/bin

NDK_ABI ?= arm64-v8a

ifeq ($(NDK_ABI),armeabi-v7a)
  NDK_TARGET = armv7a-linux-androideabi
else ifeq ($(NDK_ABI),arm64-v8a)
  NDK_TARGET = aarch64-linux-android
else ifeq ($(NDK_ABI),x86)
  NDK_TARGET = i686-linux-android
else ifeq ($(NDK_ABI),x86_64)
  NDK_TARGET = x86_64-linux-android
else
  $(error Unknown NDK_ABI)
endif

NDK_VERSION ?= 21
CC = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang
CXX = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang++
LD = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang++

SFLAGS += -fPIC

#GDB = arm-none-eabi-gdb
#OBJCOPY = arm-none-eabi-objcopy
#SIZE = arm-none-eabi-size


#SFLAGS += -fdata-sections -ffunction-sections
#LDFLAGS += -Wl,--gc-sections
#SFLAGS += -fvisibility=hidden
#LDFLAGS += -Wl,-export-symbols,ion/src/sdl/export_list.txt

LDFLAGS += -shared
LDFLAGS += -static-libstdc++
#LDFLAGS += -Wl,-z,defs # Warn on missing symbols when linking the dynamic library

#LDFLAGS += -lsdl
