TARGET = thumbv7em-unknown-eabi
MCUFLAGS = -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16
# There is no armv7m_soft_fpv5_sp_d16 but there is fpv4
# fpv5 is only a small superset of fpv4 so it’s not a big deal
LIBCLANG_ARCH = armv7m_hard_fpv4_sp_d16

include $(PATH_haussmann)/src/toolchains/shared.arm-clang.mak
