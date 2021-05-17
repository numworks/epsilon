INFO_API_LEVEL := 0

objects = $(addprefix numwars/,\
  alien.o \
  life.o \
  rocket.o \
  spaceship.o \
)

objects += $(addprefix utils/,\
  app_info.o \
  memcpy.o \
  memset.o \
  svc.o \
)

objects += main.o icon.o

custom_application.bin: custom_application.elf
	arm-none-eabi-objcopy -O binary $^ $@

custom_application.elf: $(objects) flash.ld
	arm-none-eabi-gcc -Wl,--gc-sections -Os -Ilib -I. -Wall -MD -MP -ggdb3 -fdata-sections -ffunction-sections -fpie -mthumb -mfloat-abi=hard -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -ffreestanding -nostdinc -nostdlib -lgcc -Wl,-T,flash.ld $(filter-out %.ld,$^) -o $@

%.o: %.cpp icon.h
	arm-none-eabi-g++ -std=c++11 -fno-exceptions -fno-rtti -fno-threadsafe-statics -Os -DINFO_API_LEVEL=$(INFO_API_LEVEL) -I. -Wall -MD -MP -ggdb3 -fdata-sections -ffunction-sections -fpie -mthumb -mfloat-abi=hard -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -ffreestanding -nostdinc -nostdlib -c $(filter-out icon.h,$^) -o $@

.PRECIOUS: icon.h icon.cpp
icon.h icon.cpp: icon.png
	python3 utils/inliner.py --png $< --header $(basename $@).h --cimplementation $(basename $@).cpp

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -f *.d *.o **/*.d **/*.o icon.h icon.cpp *.elf *.bin
