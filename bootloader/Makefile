
bootloader_src += $(addprefix bootloader/,\
  main.cpp \
  slot.cpp \
  interface.cpp \
  jump_to_firmware.s \
  trampoline.cpp \
)

bootloader_images = $(addprefix bootloader/, \
  cable.png \
  computer.png \
)

bootloader_src += $(ion_src) $(kandinsky_src) $(liba_src) $(libaxx_src) $(bootloader_images)

$(eval $(call depends_on_image,bootloader/interface.cpp,$(bootloader_images)))
