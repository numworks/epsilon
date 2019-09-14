src += $(addprefix ion/src/sdl/shared/, \
  language_dummy.cpp \
  telemetry_dummy.cpp \
)

src += $(addprefix ion/src/sdl/linux/, \
  images.cpp \
)

$(eval $(call rule_for, \
  OBJCOPY, \
  ion/src/sdl/assets/background.o, \
  ion/src/sdl/assets/background.jpg, \
  $$(OBJCOPY) -I binary -O elf64-x86-64 -B i386 $$^ $$@ \
))

objs += build/sdl/linux/ion/src/sdl/assets/background.o
