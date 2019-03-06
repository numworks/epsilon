# CAUTION: All the following builds have to happen in a sequential order so we
# cannot use standard Make dependencies
.PHONY: dependencies
dependencies:
	rm -rf ion/src/sdl/android/libs
	# First, build libsdl
	cd $(SDL_PATH) && build-scripts/androidbuildlibs.sh
	cp -r $(SDL_PATH)/build/android/lib ion/src/sdl/android/libs
	# Then build epsilon for armv7
	rm -f ion/src/sdl/android/libs/armeabi-v7a/libepsilon.so
	$(MAKE) ion/src/sdl/android/libs/armeabi-v7a/libepsilon.so
	# Then for arm64
	rm -f ion/src/sdl/android/libs/arm64-v8a/libepsilon.so
	$(MAKE) ion/src/sdl/android/libs/arm64-v8a/libepsilon.so
	# Then for x86
	rm -f ion/src/sdl/android/libs/x86/libepsilon.so
	$(MAKE) ion/src/sdl/android/libs/x86/libepsilon.so
	# And eventually for x86_64
	rm -f ion/src/sdl/android/libs/x86_64/libepsilon.so
	$(MAKE) ion/src/sdl/android/libs/x86_64/libepsilon.so
	# Last but not least, copy the sources from SDL
	cp -r $(SDL_PATH)/android-project/app/src/main/java/org ion/src/sdl/android/src/java

epsilon.apk:
	#rm ion/src/sdl/android/com.numworks.calculator/app/libs/arm64-v8a/libepsilon.so
	#$(MAKE) ion/src/sdl/android/com.numworks.calculator/app/libs/arm64-v8a/libepsilon.so
	cd ion/src/sdl/android ; ANDROID_HOME=/usr/local/android gradle assembleRelease
	cp ion/src/sdl/android/build/outputs/apk/release/app-release-unsigned.apk epsilon.apk

epsilon_run:
	#rm ion/src/sdl/android/com.numworks.calculator/app/libs/arm64-v8a/libepsilon.so
	#$(MAKE) ion/src/sdl/android/com.numworks.calculator/app/libs/arm64-v8a/libepsilon.so
	cd ion/src/sdl/android ; ANDROID_HOME=/usr/local/android gradle installDebug
