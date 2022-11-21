# The following lines allow us to use our own SDL_config.h

# First, make sure an error is raised if we ever use the standard SDL_config.h
SFLAGS += -DUSING_GENERATED_CONFIG_H
# Then use our very own include dir if either SDL.h or SDL_config.h are included
# Note that this one will appear before ion/src/simulator/external/sdl/include
# which is added later on in the SDL Makefile
SFLAGS += -Iion/src/simulator/web/include

# Only render the screen, not the whole calculator which will be drawn in HTML
SFLAGS += -DEPSILON_SDL_SCREEN_ONLY=1

# Enable to set environment variables for a module
LDFLAGS += --pre-js ion/src/simulator/web/preamble_env.js

ion_src += $(addprefix ion/src/simulator/web/, \
  clipboard_helper.cpp \
  exports.cpp \
  journal.cpp \
  keyboard_callback.cpp \
  window_callback.cpp \
)

ion_src += $(addprefix ion/src/simulator/shared/, \
  dummy/circuit_breaker.cpp \
  dummy/language.cpp \
  dummy/haptics_enabled.cpp \
  haptics.cpp \
)

ion_src += ion/src/shared/collect_registers.cpp

ifeq ($(EPSILON_TELEMETRY),1)
ion_src += ion/src/simulator/shared/dummy/telemetry_init.cpp
ion_src += ion/src/shared/telemetry_console.cpp
endif

DEFAULT = epsilon.zip

$(addprefix $(BUILD_DIR)/ion/src/simulator/web/,calculator.html calculator.css): ion/src/simulator/shared/layout.json ion/src/simulator/web/css_html_layout.py | $$(@D)/.
	$(call rule_label,LAYOUT)
	$(Q) $(PYTHON) ion/src/simulator/web/css_html_layout.py --html $(BUILD_DIR)/ion/src/simulator/web/calculator.html --css $(BUILD_DIR)/ion/src/simulator/web/calculator.css $<

$(BUILD_DIR)/ion/src/simulator/web/simulator.html: $(addprefix $(BUILD_DIR)/ion/src/simulator/web/,calculator.html calculator.css)

$(BUILD_DIR)/epsilon%zip: $(BUILD_DIR)/epsilon%js $(BUILD_DIR)/ion/src/simulator/web/simulator.html $(BUILD_DIR)ion/src/simulator/assets/background.jpg
	@rm -rf $(basename $@)
	@mkdir -p $(basename $@)
	@cp $< $(basename $@)/epsilon.js
	@cp ion/src/simulator/assets/background-with-shadow.webp $(basename $@)/
	@cp $(BUILD_DIR)/ion/src/simulator/assets/background.jpg $(basename $@)/
	@cp $(BUILD_DIR)/ion/src/simulator/web/simulator.html $(basename $@)/
	$(call rule_label,ZIP)
	@zip -r -9 -j $@ $(basename $@) > /dev/null
	@rm -rf $(basename $@)
