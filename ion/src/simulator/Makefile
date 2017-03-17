objs += $(addprefix ion/src/simulator/, \
  battery.o\
  init.o\
)

objs += $(addprefix ion/src/simulator/boot/, main.o)
objs += $(addprefix ion/src/simulator/display/, fltklcd.o)
objs += $(addprefix ion/src/simulator/keyboard/, fltkkbd.o)

objs += $(addprefix ion/src/shared/, \
  crc32.o \
  events_keyboard.o \
  power.o \
  random.o \
)

SFLAGS += `fltk-config --cflags`
LDFLAGS += `fltk-config --ldflags`

.PHONY: %_memory_map
%_memory_map:
	@echo "========== MEMORY MAP ========="
	@echo "No memory map on simulator."
	@echo "==============================="
