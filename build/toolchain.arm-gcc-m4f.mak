include build/toolchain.arm-gcc.mak
SFLAGS += -mthumb -march=armv7e-m -mfloat-abi=hard
SFLAGS += -mcpu=cortex-m4 -mfpu=fpv4-sp-d16
