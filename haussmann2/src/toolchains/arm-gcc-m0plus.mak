include $(PATH_haussmann)/src/toolchains/shared.arm-gcc.mak

SFLAGS += \
  -mcpu=cortex-m0plus \
  -mfloat-abi=soft \
  -mthumb
