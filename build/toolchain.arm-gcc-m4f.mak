include build/toolchain.arm-gcc.mak
SFLAGS += -mthumb -mfloat-abi=hard
SFLAGS += -mcpu=cortex-m4 -mfpu=fpv4-sp-d16
