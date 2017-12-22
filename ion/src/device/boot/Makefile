objs += $(addprefix ion/src/device/boot/, isr.o rt0.o)
LDFLAGS += -T ion/src/device/boot/flash.ld

LDFLAGS += -Wl,-M -Wl,-Map=output.map
products += output.map

.PHONY: %_memory_map
%_memory_map: %.elf
	@echo "========== MEMORY MAP ========="
	@awk -f ion/src/device/boot/memory_map.awk < output.map
	@echo "==============================="
