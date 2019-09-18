include build/toolchain.arm-gcc.mak
SFLAGS += -mthumb -march=armv7e-m -mfloat-abi=hard
SFLAGS += -mcpu=cortex-m7 -mfpu=fpv5-sp-d16
