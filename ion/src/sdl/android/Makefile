src += $(addprefix ion/src/sdl/android/src/cpp/, \
  images.cpp \
)

LDFLAGS += -ljnigraphics -llog

ANDROID_LIB_PATH = ion/src/sdl/android/libs

$(ANDROID_LIB_PATH)/%/libepsilon.so:
ion/src/sdl/android/libs/%/libepsilon.so:
	$(Q) $(MAKE) MODEL=android NDK_ABI=$* clean
	$(Q) $(MAKE) MODEL=android NDK_ABI=$* epsilon.so
	$(Q) mv epsilon.so $@
