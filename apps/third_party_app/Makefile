INFO_API_LEVEL := 0

objects = main.o icon.o svc.o app_info.o

custom_application.bin: custom_application.elf
	arm-none-eabi-objcopy -O binary $^ $@

custom_application.elf: $(objects) flash.ld
	arm-none-eabi-gcc -Wl,--gc-sections -O0 -g -Ilib -I. -Wall -MD -MP -ggdb3 -fdata-sections -ffunction-sections -fpie -mthumb -mfloat-abi=hard -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -ffreestanding -nostdinc -nostdlib -lgcc -Wl,-M -Wl,-T,flash.ld $(filter-out %.ld,$^) -o $@

%.o: %.cpp
	arm-none-eabi-g++ -std=c++11 -fno-exceptions -fno-rtti -fno-threadsafe-statics -O0 -g -DINFO_API_LEVEL=$(INFO_API_LEVEL) -I. -Wall -MD -MP -ggdb3 -fdata-sections -ffunction-sections -fpie -mthumb -mfloat-abi=hard -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -ffreestanding -nostdinc -nostdlib -c $^ -o $@

.PRECIOUS: icon.h icon.cpp
maapp_info.cpp: icon.h
icon.h icon.cpp: icon.png
	python3 inliner.py --png $< --header $(basename $@).h --cimplementation $(basename $@).cpp

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -f *.d *.o icon.h icon.cpp *.elf *.bin
