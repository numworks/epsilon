TOOLCHAIN = emscripten
USE_LIBA = 0
EXE = js
OS_WITH_ONBOARDING_APP ?= 0
OS_WITH_SOFTWARE_UPDATE_PROMPT ?= 0

numworks_simulator.zip: LDFLAGS += --memory-init-file 0
numworks_simulator.zip: app.js
	@rm -rf $(basename $@)
	@mkdir $(basename $@)
	@cp app.js $(basename $@)/firmware.js
	@cp ion/src/emscripten/background.jpg $(basename $@)/
	@cp ion/src/emscripten/simulator.html $(basename $@)/
	@echo "ZIP     $@"
	@zip -r -9 $@ $(basename $@) > /dev/null
	@rm -rf $(basename $@)
