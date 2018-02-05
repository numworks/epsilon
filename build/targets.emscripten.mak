epsilon.packed.js: LDFLAGS += --memory-init-file 0
epsilon.packed.js: $(app_objs) $(app_image_objs)

simulator.zip: epsilon.packed.js
	@rm -rf $(basename $@)
	@mkdir $(basename $@)
	@cp epsilon.packed.js $(basename $@)/epsilon.js
	@cp ion/src/emscripten/background.jpg $(basename $@)/
	@cp ion/src/emscripten/simulator.html $(basename $@)/
	@echo "ZIP     $@"
	@zip -r -9 $@ $(basename $@) > /dev/null
	@rm -rf $(basename $@)

products += $(addprefix epsilon,.js .js.mem) epsilon.packed.js simulator.zip
