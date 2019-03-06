$(BUILD_DIR)/epsilon.packed.js: LDFLAGS += --memory-init-file 0
$(BUILD_DIR)epsilon.packed.js: $(objs) $(app_objs) $(app_image_objs)

$(BUILD_DIR)/simulator.zip: epsilon.packed.js
	@rm -rf $(basename $@)
	@mkdir $(basename $@)
	@cp epsilon.packed.js $(basename $@)/epsilon.js
	@cp ion/src/emscripten/background.jpg $(basename $@)/
	@cp ion/src/emscripten/simulator.html $(basename $@)/
	@echo "ZIP     $@"
	@zip -r -9 $@ $(basename $@) > /dev/null
	@rm -rf $(basename $@)
