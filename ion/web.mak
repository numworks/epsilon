SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  shared/dummy/circuit_breaker.cpp \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/language.cpp \
  shared/haptics.cpp \
  web/clipboard_helper.cpp \
  web/exports.cpp \
  web/journal.cpp \
  web/keyboard_callback.cpp \
  web/window_callback.cpp \
)

SOURCES_ion += $(PATH_ion)/src/shared/collect_registers.cpp

PRIVATE_SFLAGS_ion += -DEPSILON_SDL_SCREEN_ONLY=1

LDFLAGS_ion += --pre-js ion/src/simulator/web/preamble_env.js

_ion_simulator_files := 0

# HTML layout

_ion_web_path := $(PATH_ion)/src/simulator/web

$(OUTPUT_DIRECTORY)/$(_ion_web_path)/calculator.html: $(addprefix $(PATH_ion)/src/simulator/,shared/layout.json web/css_html_layout.py) | $$(@D)/.
	$(call rule_label,LAYOUT)
	$(QUIET) $(PYTHON) $(filter %.py,$^) --html $@ --css $(basename $@).css $(filter %.json,$^)

$(OUTPUT_DIRECTORY)/$(_ion_web_path)/calculator.css: $(OUTPUT_DIRECTORY)/$(_ion_web_path)/calculator.html

$(OUTPUT_DIRECTORY)/$(_ion_web_path)/simulator.html: $(_ion_web_path)/simulator.html.inc $(addprefix $(OUTPUT_DIRECTORY)/$(_ion_web_path)/calculator.,html css)
	$(call rule_label,HOSTCPP)
	$(QUIET) $(HOSTCPP) -I$(dir $@) -P $(filter %.inc,$^) $@
