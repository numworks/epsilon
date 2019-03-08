src += $(addprefix ion/src/sdl/android/src/cpp/, \
  images.cpp \
)

$(call object_for,ion/src/sdl/shared/main.cpp) : SFLAGS += -DEPSILON_SDL_FULLSCREEN=1

LDFLAGS += -ljnigraphics -llog

ANDROID_LIB_PATH = ion/src/sdl/android/libs

$(ANDROID_LIB_PATH)/%/libepsilon.so:
ion/src/sdl/android/libs/%/libepsilon.so:
	$(Q) $(MAKE) MODEL=android NDK_ABI=$* clean
	$(Q) $(MAKE) MODEL=android NDK_ABI=$* epsilon.so
	$(Q) mv epsilon.so $@
