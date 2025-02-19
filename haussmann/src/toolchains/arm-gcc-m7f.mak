include $(PATH_haussmann)/src/toolchains/shared.arm-gcc.mak

SFLAGS += \
  -mcpu=cortex-m7 \
  -mfloat-abi=hard \
  -mfpu=fpv5-sp-d16 \
  -mthumb
