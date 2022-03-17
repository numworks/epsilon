
bootloader_src += $(addprefix bootloader/,\
  boot.cpp \
  main.cpp \
  kernel_header.cpp \
  userland_header.cpp \
  slot.cpp \
  interface.cpp \
  jump_to_firmware.s \
  trampoline.cpp \
  usb_desc.cpp \
)

bootloader_images = $(addprefix bootloader/, \
  cable.png \
  computer.png \
)

bootloader_src += $(filter-out ion/src/device/shared/drivers/usb_desc.cpp,$(ion_src)) $(simple_kandinsky_src) $(liba_src) $(libaxx_src) $(bootloader_images)

$(eval $(call depends_on_image,bootloader/interface.cpp,$(bootloader_images)))
