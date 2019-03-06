$(BUILD_DIR)/app/libs/%/libepsilon.so: $$(@D)/.
	$(Q) echo "MAKE    NDK_ABI=$*"
	$(Q) $(MAKE) NDK_ABI=$* epsilon.so
	$(Q) cp build/sdl/android/$*/epsilon.so $@

NDK_ABIS = arm64-v8a x86_64

.PHONY: epsilon.apk
epsilon.apk: $(patsubst %,$(BUILD_DIR)/app/libs/%/libepsilon.so,$(NDK_ABIS))
	@echo "GRADLE ion/src/sdl/android/build.gradle"
	$(Q) ANDROID_HOME=/usr/local/android gradle -b ion/src/sdl/android/build.gradle assembleRelease

.PHONY: epsilon_run
epsilon_run:
	ANDROID_HOME=/usr/local/android gradle -b ion/src/sdl/android/build.gradle assembleRelease
	#rm ion/src/sdl/android/com.numworks.calculator/app/libs/arm64-v8a/libepsilon.so
	#$(MAKE) ion/src/sdl/android/com.numworks.calculator/app/libs/arm64-v8a/libepsilon.so
	cd ion/src/sdl/android ; ANDROID_HOME=/usr/local/android gradle installDebug
