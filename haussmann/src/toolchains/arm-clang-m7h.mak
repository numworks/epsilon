TARGET = thumbv7em-unknown-eabi
MCUFLAGS = -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16
LIBCLANG_ARCH = armv7m_hard_fpv5_d16

include $(PATH_haussmann)/src/toolchains/shared.arm-clang.mak
