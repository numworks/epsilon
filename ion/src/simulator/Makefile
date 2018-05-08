objs += $(addprefix ion/src/simulator/, \
  init.o\
  led.o\
)

objs += $(addprefix ion/src/simulator/boot/, main.o)
objs += $(addprefix ion/src/simulator/display/, fltklcd.o)
objs += $(addprefix ion/src/simulator/keyboard/, fltkkbd.o)

objs += $(addprefix ion/src/shared/, \
  crc32.o \
  console_line.o \
  console_stdio.o \
  events_modifier.o \
  power.o \
  random.o \
  dummy/backlight.o \
  dummy/battery.o \
  dummy/fcc_id.o \
  dummy/serial_number.o \
  dummy/usb.o \
)

# FLTK Image pre-processor
FLUID := fluid

%.h %.cpp : %.fl
	@echo "FLUID   $@"
	@$(FLUID) -c $<
	@mv -f *_icon.* ion/src/simulator/keyboard/

fl_files += $(addprefix ion/src/simulator/keyboard/, kbdimage_icon.fl)

fl_file_objs := $(fl_files:.fl=.o)
.SECONDARY: $(fl_files:.fl=.cpp)
$(objs): $(fl_file_objs)

objs +=  $(fl_file_objs)

FLUID_PRODUCTS = $(1:.fl=.h) $(1:.fl=.cpp) $(1:.fl=.o)

products += $(call FLUID_PRODUCTS,$(fl_files))

SFLAGS += `fltk-config --cflags`
LDFLAGS += `fltk-config --ldflags`

.PHONY: %_memory_map
%_memory_map:
	@echo "========== MEMORY MAP ========="
	@echo "No memory map on simulator."
	@echo "==============================="
