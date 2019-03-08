$(BUILD_DIR)/%/epsilon.bin:
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

epsilon_run:
	xcrun simctl install C75D15A1-45F7-4F94-8DBD-6D02A95C9514 $(BUILD_DIR)/app/Epsilon.app
