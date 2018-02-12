objs += $(addprefix ion/src/device/boot/, isr.o rt0.o)

ion/src/device/boot/%.o: SFLAGS += -fno-lto

LDFLAGS += -T ion/src/device/boot/flash.ld
