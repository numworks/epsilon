include build/toolchain.arm-gcc.mak
SFLAGS += -mthumb -mfloat-abi=hard
SFLAGS += -mcpu=cortex-m7 -mfpu=fpv5-sp-d16
