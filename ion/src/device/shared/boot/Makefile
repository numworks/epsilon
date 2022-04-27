ifeq ($(filter bootloader, ${MAKECMDGOALS}), bootloader)
ion_device_src += $(addprefix bootloader/boot/, \
  isr.c \
)
else
ion_device_src += $(addprefix ion/src/device/shared/boot/, \
  isr.c \
)
endif
