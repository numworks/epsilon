
bootloader_src += $(addprefix bootloader/,\
  slot_exam_mode.cpp \
  boot.cpp \
  main.cpp \
  kernel_header.cpp \
  userland_header.cpp \
  slot.cpp \
  interface.cpp \
  jump_to_firmware.s \
  trampoline.cpp \
  recovery.cpp \
  usb_data.cpp \
  itoa.cpp \
  utility.cpp \
)

bootloader_images = $(addprefix bootloader/, \
  computer.png \
)

bootloader_src += $(addprefix bootloader/interface/src/,\
  menu.cpp \
)

bootloader_src += $(addprefix bootloader/interface/menus/about/,\
  about.cpp \
)

bootloader_src += $(addprefix bootloader/interface/menus/home/,\
  home.cpp \
)


bootloader_src += $(ion_src) $(simple_kandinsky_src) $(liba_src) $(libaxx_src) $(bootloader_images)

$(eval $(call depends_on_image,bootloader/interface.cpp,$(bootloader_images)))
$(eval $(call depends_on_image,bootloader/interface/menus/home/home.cpp,$(bootloader_images)))
