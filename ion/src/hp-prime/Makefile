objs += $(addprefix ion/src/hp-prime/boot/, crt0.o rt0.o)

objs += $(addprefix ion/src/shared/, \
  crc32.o \
  events_modifier.o \
  power.o \
  dummy/battery.o \
  dummy/fcc_id.o \
  dummy/random.o \
  dummy/serial_number.o \
  dummy/usb.o \
)

objs += $(addprefix ion/src/hp-prime/, \
  device.o\
  display.o\
  events.o\
  keyboard.o\
  led.o \
)

LDFLAGS += -T ion/src/hp-prime/boot/flash.ld

%.bin: %.$(EXE)
	@echo "OBJCOPY $@"
	$(Q) $(OBJCOPY) -O binary --pad-to 0x30100000 $< $@
