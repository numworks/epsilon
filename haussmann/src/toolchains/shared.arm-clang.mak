CC := clang
CXX := clang++
LD := clang
GDB := arm-none-eabi-gdb
OBJCOPY := llvm-objcopy

EXECUTABLE_EXTENSION := elf

COMPILER_FAMILY := clang

DFU ?= dfu-util

SFLAGS += -target $(TARGET) $(MCUFLAGS)
CFLAGS += -fshort-enums
CXXFLAGS += -fshort-enums -Wno-invalid-constexpr -Wno-inline-new-delete

ifndef LIBCLANG_ROOT
$(error You must set a LIBCLANG_ROOT that provides libclang_rt.builtins.a for armv7m architectures. Download ST commandline tools and pass stm32cubeclt_1.20.0/st-arm-clang/lib/clang-runtimes/ for instance.)
endif

LDFLAGS += \
  -fuse-ld=lld \
  -Wl,--undefined-glob,__aeabi_memcpy* \
  -Wl,--undefined-glob,__aeabi_memclr* \
  -Wl,--undefined-glob,__aeabi_fsub \
  -Wl,--undefined-glob,__aeabi_dsub \
  $(LIBCLANG_ROOT)/arm-none-eabi/$(LIBCLANG_ARCH)/lib/libclang_rt.builtins.a

SFLAGS += \
  -fno-builtin \
  -fdata-sections \
  -ffreestanding \
  -ffunction-sections \
  -ggdb3 \
  -nostdinc \
  -nostdlib

ifeq ($(DEBUG),0)
LDFLAGS += -Wl,--gc-sections
SFLAGS := $(filter-out -Os,$(SFLAGS)) -Oz -flto=full
endif
