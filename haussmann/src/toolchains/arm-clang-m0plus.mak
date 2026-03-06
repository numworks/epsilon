TARGET = thumbv6m-unknown-eabi
MCUFLAGS = -mcpu=cortex-m0plus
LIBCLANG_ARCH = armv6m_soft_nofp

include $(PATH_haussmann)/src/toolchains/shared.arm-clang.mak
