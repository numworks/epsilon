# Makefile is loaded for a single architecture at once. If NDK_ABI is not
# defined, we assume we want to build all of them. But in that case, we cannot
# track per-ABI dependencies. So as a shortcut, we force a re-make of each ABI.

.PHONY: force_rebuild_libepsilon

$(BUILD_DIR)/app/libs/%/libepsilon.so: force_rebuild_libepsilon
	$(Q) echo "MAKE    NDK_ABI=$*"
	$(Q) $(MAKE) NDK_ABI=$* epsilon.so
	$(Q) cp $(BUILD_DIR)/$*/epsilon.so $@

NDK_ABIS = arm64-v8a x86_64

.PHONY: epsilon.apk
epsilon.apk: $(patsubst %,$(BUILD_DIR)/app/libs/%/libepsilon.so,$(NDK_ABIS))
	@echo "GRADLE ion/src/sdl/android/build.gradle"
	$(Q) ANDROID_HOME=/usr/local/android gradle -b ion/src/sdl/android/build.gradle assembleRelease

.PHONY: epsilon_run
epsilon_run: $(patsubst %,$(BUILD_DIR)/app/libs/%/libepsilon.so,$(NDK_ABIS))
	@echo "GRADLE ion/src/sdl/android/build.gradle"
	$(Q) ANDROID_HOME=/usr/local/android gradle -b ion/src/sdl/android/build.gradle installDebug
