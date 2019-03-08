.PHONY: force_rebuild_libepsilon

$(BUILD_DIR)/%/epsilon.bin: force_rebuild_libepsilon
	$(Q) echo "MAKE    ARCH=$*"
	$(Q) $(MAKE) ARCH=$*

ARCHS ?= x86_64

.PHONY: epsilon.ipa
epsilon.ipa: $(BUILD_DIR)/app/Epsilon.app

$(BUILD_DIR)/app/Epsilon.app: $(patsubst %,$(BUILD_DIR)/%/epsilon.bin,$(ARCHS))
	rm -rf $@
	mkdir -p $@
	lipo -create $^ -output $@/Epsilon
	cp ion/src/sdl/ios/Info.plist $@/
	cp ion/src/sdl/assets/* $@/
	ibtool --compile $@/launch.storyboardc ion/src/sdl/ios/launch.storyboard

epsilon_run: $(BUILD_DIR)/app/Epsilon.app
	xcrun simctl install booted $(BUILD_DIR)/app/Epsilon.app
