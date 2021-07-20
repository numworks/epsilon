Q ?= @
SFLAGS = -I. -Wall -MD -MP -ggdb3 -mthumb -mfloat-abi=hard -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -Isrc
CFLAGS = -Os -S -fdata-sections -ffunction-sections -fpie -fno-exceptions -ffreestanding

%.s: %.c
	@echo "C      $^"
	$(Q) arm-none-eabi-gcc $(CFLAGS) $(SFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm *.s *.d
