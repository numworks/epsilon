objs += $(addprefix ion/src/sdl/android/, \
  images.o \
)

ANDROID_LIB_PATH = ion/src/sdl/android/com.numworks.calculator/app/libs

$(ANDROID_LIB_PATH)/%/libepsilon.so:
ion/src/sdl/android/com.numworks.calculator/app/libs/%/libepsilon.so:
	$(Q) $(MAKE) MODEL=android NDK_ABI=$* clean
	$(Q) $(MAKE) MODEL=android NDK_ABI=$* epsilon.so
	$(Q) mv epsilon.so $@
