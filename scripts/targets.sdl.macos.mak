.PHONY: force_rebuild_libepsilon

$(BUILD_DIR)/%/epsilon.bin: force_rebuild_libepsilon
	$(Q) echo "MAKE    ARCH=$*"
	$(Q) $(MAKE) ARCH=$*

ARCHS ?= x86_64

.PHONY: epsilon.ipa
epsilon.ipa: $(BUILD_DIR)/app/Epsilon.app

$(BUILD_DIR)/app/Epsilon.app: $(patsubst %,$(BUILD_DIR)/%/epsilon.bin,$(ARCHS))
	rm -rf $@
	mkdir -p $@/Contents/MacOS
	lipo -create $^ -output $@/Contents/MacOS/Epsilon
	cp ion/src/sdl/macos/Info.plist $@/Contents
	mkdir -p $@/Contents/Resources
	cp ion/src/sdl/assets/* $@/Contents/Resources

epsilon_run:
	open $(BUILD_DIR)/app/Epsilon.app
