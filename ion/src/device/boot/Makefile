objs += $(addprefix ion/src/device/boot/, isr.o crt0.o)
LDFLAGS += -T ion/src/device/boot/flash.ld

ifeq ($(VERBOSE),1)
  LDFLAGS += -M -Map output.map
  products += output.map
.PHONY: memory_map
postbuild: memory_map
memory_map:
	@echo "========== MEMORY MAP ========="
	@awk -f boot/device/memory_map.awk < output.map
	@echo "==============================="
endif

