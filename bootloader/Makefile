
bootloader_src += $(addprefix bootloader/,\
  utility.cpp \
  boot.cpp \
  main.cpp \
  jump_to_firmware.s \
  trampoline.cpp \
  recovery.cpp \
  usb_data.cpp \
)

bootloader_src += $(addprefix bootloader/slots/, \
  slot_exam_mode.cpp \
  slot.cpp \
  userland_header.cpp \
  kernel_header.cpp \
)

bootloader_src += $(addprefix bootloader/drivers/, \
  stm32_drivers.cpp \
)

bootloader_src += $(addprefix bootloader/interface/static/, \
  interface.cpp \
)

bootloader_src += $(addprefix bootloader/interface/src/,\
  menu.cpp \
)


bootloader_src += $(addprefix bootloader/interface/menus/, \
  about.cpp \
  home.cpp \
  dfu.cpp \
  installer.cpp \
  warning.cpp \
  slot_recovery.cpp \
  crash.cpp \
)

bootloader_images = $(addprefix bootloader/, \
  computer.png \
)

bootloader_src += $(ion_src) $(simple_kandinsky_src) $(liba_src) $(libaxx_src) $(bootloader_images)

$(eval $(call depends_on_image,bootloader/interface/static/interface.cpp,$(bootloader_images)))
