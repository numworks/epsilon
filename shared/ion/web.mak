# Command line flags :
# WEB_EXTERNAL_APP: add support for nwb files loaded with dlopen

SOURCES_ion += $(addprefix $(PATH_ion)/src/simulator/, \
  shared/dummy/haptics_enabled.cpp \
  shared/dummy/language.cpp \
  shared/haptics.cpp \
  shared/state_file.cpp \
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

_ion_web_exported_functions = $(subst $( ),$(,),$(strip $(patsubst %,_%, \
  main \
  IonSimulatorKeyboardKeyDown \
  IonSimulatorKeyboardKeyUp \
  IonSimulatorEventsPushEvent \
  IonSimulatorEventsPushEventArray \
  IonSimulatorNumberOfEventsInArray \
  IonSoftwareVersion \
  IonPatchLevel \
  malloc \
  free \
  $(_eadk_exported_functions) \
)))

# Using = instead of := is needed here as there is no garantee, the variable
# $(_sdl_web_exported_runtime_methods) is already defined
_ion_web_exported_runtime_methods = $(subst $( ),$(,),$(strip \
  UTF8ToString \
  HEAPU8 \
  FS \
  $(_sdl_web_exported_runtime_methods) \
))

LDFLAGS_ion += \
  --pre-js $(PATH_ion)/src/simulator/web/preamble_env.js \
  -sEXPORTED_FUNCTIONS=$(_ion_web_exported_functions) \
  -sEXPORTED_RUNTIME_METHODS=$(_ion_web_exported_runtime_methods)

ifeq ($(DEBUG),1)
# Mandelbrot crashes with 8192, is 16384 enough for all scripts ?
LDFLAGS += -sASYNCIFY_STACK_SIZE=16384
else
endif

ifeq ($(WEB_EXTERNAL_APPS),1)
_ion_external_apps := 1
# This flags allows side modules (compiled with SIDE_MODULE=2) to be dynamically
# loaded.
SFLAGS_ion += -sMAIN_MODULE=2
else
_ion_external_apps := 0
endif

_ion_simulator_files := 0

# HTML layout

_ion_web_path := $(PATH_ion)/src/simulator/web

$(call generated_sources_for, app/src/background-with-shadow.webp): $(PATH_ion)/src/simulator/assets/$(ION_layout_variant)/background-with-shadow.webp | $$(@D)/.
	$(call rule_label,COPY)
	cp $^ $@

$(call generated_sources_for, app/src/calculator.html): $(addprefix $(PATH_ion)/src/simulator/,shared/$(ION_layout_variant)/layout.json web/css_html_layout.py) | $$(@D)/.
	$(call rule_label,LAYOUT)
	$(PYTHON) $(filter %.py,$^) --html $@ --css $(basename $@).css $(filter %.json,$^)

$(call generated_sources_for, app/src/calculator.css): $(call generated_sources_for, app/src/calculator.html) | $$(@D)/.

$(call generated_sources_for, app/src/calculator.js): $(_ion_web_path)/calculator.js | $$(@D)/.
	$(call rule_label,HOSTCPP)
	$(HOSTCPP) \
		-E -CC \
		-DEM_MODULE_NAME=$(APP_NAME) \
		-P $(filter %.js,$^) \
		$@

$(call generated_sources_for, app/src/simulator_%html): $(_ion_web_path)/simulator.html.inc $(call generated_sources_for, $(addprefix app/src/calculator.,html css)) | $$(@D)/.
	$(call rule_label,HOSTCPP)
	$(HOSTCPP) \
                -MMD -MP \
		-I$(dir $@) \
		-MD -MP \
		-DEM_MODULE_NAME=$(APP_NAME) \
		-DEM_MODULE_JS='"$*js"' \
		-DPATCH_LEVEL=\"$(PATCH_LEVEL)\" \
		-DEPSILON_VERSION=\"$(APP_VERSION)\" \
		-DION_DISPLAY_WIDTH=$(_ion_display_width_$(ION_layout_variant)) \
		-DION_DISPLAY_HEIGHT=$(_ion_display_height_$(ION_layout_variant)) \
		-DLAYOUT_$(ION_layout_variant) \
		-P $(filter %.inc,$^) \
		$@
