SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/language.cpp \
  shared/haptics.cpp \
  web/clipboard_helper.cpp \
  web/exports.cpp \
  web/journal.cpp \
  web/keyboard_callback.cpp \
  web/window_callback.cpp \
)

SOURCES_ion += $(addprefix $(PATH_ion)/src/shared/, \
  collect_registers.cpp \
  dummy/circuit_breaker.cpp \
)

PRIVATE_SFLAGS_ion += -DEPSILON_SDL_SCREEN_ONLY=1

_ion_web_exported_functions := $(subst $( ),$(,) ,$(strip $(patsubst %,"%", \
  _main \
  _IonSimulatorKeyboardKeyDown \
  _IonSimulatorKeyboardKeyUp \
  _IonSimulatorEventsPushEvent \
  _IonSoftwareVersion \
  _IonPatchLevel \
)))

_ion_web_exported_runtime_methods := $(subst $( ),$(,) ,$(strip $(patsubst %,"%", \
  UTF8ToString \
)))

LDFLAGS_ion += \
  --pre-js $(PATH_ion)/src/simulator/web/preamble_env.js \
  -s EXPORTED_FUNCTIONS='[$(_ion_web_exported_functions)]' \
  -s EXPORTED_RUNTIME_METHODS='[$(_ion_web_exported_runtime_methods)]'

ifeq ($(DEBUG),1)
# Mandelbrot crashes with 8192, is 16384 enough for all scripts ?
LDFLAGS += -sASYNCIFY_STACK_SIZE=16384
else
endif

_ion_simulator_files := 0

# HTML layout

_ion_web_path := $(PATH_ion)/src/simulator/web

$(OUTPUT_DIRECTORY)/$(_ion_web_path)/calculator.html: $(addprefix $(PATH_ion)/src/simulator/,shared/$(ION_layout_variant)/layout.json web/css_html_layout.py) | $$(@D)/.
	$(call rule_label,LAYOUT)
	$(PYTHON) $(filter %.py,$^) --html $@ --css $(basename $@).css $(filter %.json,$^)

$(OUTPUT_DIRECTORY)/$(_ion_web_path)/calculator.css: $(OUTPUT_DIRECTORY)/$(_ion_web_path)/calculator.html

$(OUTPUT_DIRECTORY)/$(_ion_web_path)/simulator.html: $(_ion_web_path)/simulator.html.inc $(addprefix $(OUTPUT_DIRECTORY)/$(_ion_web_path)/calculator.,html css)
	$(call rule_label,HOSTCPP)
	$(HOSTCPP) \
		-I$(dir $@) \
		-MD -MP \
		-DEM_MODULE_NAME=$(APP_NAME) \
		-DEM_MODULE_JS='"$(ION_em_module_js)"' \
		-DPATCH_LEVEL=\"$(PATCH_LEVEL)\" \
		-DEPSILON_VERSION=\"$(APP_VERSION)\" \
		-DION_DISPLAY_WIDTH=$(_ion_display_width_$(ION_layout_variant)) \
		-DION_DISPLAY_HEIGHT=$(_ion_display_height_$(ION_layout_variant)) \
		-DLAYOUT_$(ION_layout_variant) \
		-P $(filter %.inc,$^) \
		$@
