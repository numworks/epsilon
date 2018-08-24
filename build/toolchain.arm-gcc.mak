CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
LD = arm-none-eabi-gcc
GDB = arm-none-eabi-gdb
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

ifeq ($(DEBUG),1)
SFLAGS += -ggdb3
else
SFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,--gc-sections
endif
SFLAGS += -mthumb -march=armv7e-m -mfloat-abi=hard -mcpu=cortex-m4 -mfpu=fpv4-sp-d16
LDFLAGS += $(SFLAGS) -lgcc -Wl,-T,$(LDSCRIPT)
