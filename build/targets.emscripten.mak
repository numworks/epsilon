app.js: LDFLAGS += -s 'EMTERPRETIFY_FILE="app.etb"'
app.js: $(app_objs) $(app_image_objs)

app.packed.js: LDFLAGS += --memory-init-file 0
app.packed.js: $(app_objs) $(app_image_objs)

numworks_simulator.zip: app.packed.js
	@rm -rf $(basename $@)
	@mkdir $(basename $@)
	@cp app.packed.js $(basename $@)/epsilon.js
	@cp ion/src/emscripten/background.jpg $(basename $@)/
	@cp ion/src/emscripten/simulator.html $(basename $@)/
	@echo "ZIP     $@"
	@zip -r -9 $@ $(basename $@) > /dev/null
	@rm -rf $(basename $@)

products += $(addprefix app.,js mem etb) app.packed.js numworks_simulator.zip
