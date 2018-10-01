objs += $(addprefix ion/src/simulator/, \
  init.o\
)

objs += $(addprefix ion/src/simulator/boot/, main.o)
objs += $(addprefix ion/src/simulator/display/, fltklcd.o)
objs += $(addprefix ion/src/simulator/keyboard/, fltkkbd.o)

objs += $(addprefix ion/src/shared/, \
  crc32.o \
  crc32_padded.o \
  console_line.o \
  console_stdio.o \
  events_modifier.o \
  power.o \
  random.o \
  timing.o \
  dummy/backlight.o \
  dummy/battery.o \
  dummy/fcc_id.o \
  dummy/led.o \
  dummy/serial_number.o \
  dummy/stack.o \
  dummy/usb.o \
)

SFLAGS += `fltk-config --cflags`
LDFLAGS += `fltk-config --ldflags`

.PHONY: %_memory_map
%_memory_map:
	@echo "========== MEMORY MAP ========="
	@echo "No memory map on simulator."
	@echo "==============================="
