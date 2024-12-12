# TODO update when making poincare its own repository

# Assert this is being run from the repository's top level, as haussmann does
# not like having .. in paths.
ifeq ($(shell find . -maxdepth 1 -name '.git'),)
$(error For now, this makefile should be run from the root of epsilon. Use "make -f poincare/Makefile ..." instead)
endif

PATH_haussmann := haussmann
APP_NAME := Poincare
APP_VERSION := 0.1.0
OUTPUT_ROOT := output
DEBUG ?= 0
PLATFORM ?= foxglove
EMIT_TSD ?= 1
# This could be activated only in DEBUG mode but:
# - it's simpler to always have it on
# - it's very lightweight
POINCARE_TREE_LOG = 1

include $(PATH_haussmann)/Makefile

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS)

ifeq ($(ASSERTIONS),0)
SFLAGS += -DNDEBUG
endif

# Modules

# FIXME: We shouldn't need Kandinsky font
KANDINSKY_font_variant := epsilon

# FIXME: Flags to build ion files
SFLAGS += \
  -Iion/include/ion/keyboard/epsilon \
  -DION_DISPLAY_WIDTH=1000 \
  -DION_DISPLAY_HEIGHT=1000

$(call import_module,omg,omg)
$(call import_module,kandinsky,kandinsky)
$(call import_module,poincare,poincare)
$(call import_module,python,python)

# FIXME Fake a module with enough glue to build. Eventually, we should be able
# to build poincarejs without ion.
$(call import_module,poincarejs,.)
$(call create_module,poincarejs,1, \
$(addprefix poincare/poincarejs/, \
  main.cpp \
  timing.cpp \
) \
$(addprefix ion/src/shared/, \
  console_line.cpp \
  display_context.cpp \
  events.cpp \
  events_modifier.cpp \
  exam_mode.cpp \
  keyboard.cpp \
  keyboard_queue.cpp \
  layout_events/epsilon/layout_events.cpp \
  stack_position.cpp \
) \
$(addprefix ion/src/shared/dummy/, \
  authentication.cpp \
  backlight.cpp \
  battery.cpp \
  circuit_breaker.cpp \
  clipboard.cpp \
  collect_registers.cpp \
  compilation_flags.cpp \
  console.cpp \
  display_misc.cpp \
  display_rect.cpp \
  events.cpp \
  exam_bytes.cpp \
  external_apps.cpp \
  fcc_id.cpp \
  init.cpp \
  keyboard.cpp \
  led.cpp \
  platform_info.cpp \
  post_and_hardware_tests.cpp \
  power.cpp \
  reset.h \
  stack.cpp \
  usb.cpp \
) \
$(addprefix ion/src/simulator/shared/, \
  crc32.cpp \
  device_name.cpp \
  random.cpp \
) \
)

# TODO Phase out TARGET_POINCARE_JS
SFLAGS_poincarejs += \
  -DTARGET_POINCARE_JS \
  -Iion/include

PRIVATE_SFLAGS_poincarejs += \
  -DEPSILON_VERSION=\"$(APP_VERSION)\" \
  -DPATCH_LEVEL=\"$(PATCH_LEVEL)\"

# Generate ts types file
# For this to work, go to emsdk/upstream/emscripten and run:
# > npm install typescript
# See https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#typescript-definitions
ifeq ($(EMIT_TSD),1)
LDFLAGS_poincarejs += --emit-tsd poincare.d.ts
endif

# When the final poincare.js is built, the pre/post-js files are
# concatenated to it like so:
#
# <extern-pre-js>
# var Poincare = (() => {
#   return async function () {
#     <pre-js>
#     // Main poincare.js code
#     <post-js>
#   }
#})();
# <extern-post-js>
LDFLAGS_poincarejs += --pre-js poincare/poincarejs/pre-js.js
LDFLAGS_poincarejs += --extern-pre-js poincare/poincarejs/extern-pre-js.js

LDFLAGS_poincarejs += \
  -lembind \
  -s EXPORT_ES6=1 \

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_poincarejs)): $(KANDINSKY_fonts_dependencies)

# Rules

$(call create_goal,poincare, \
  kandinsky \
  omg \
  poincare.js_bridge.nostorage \
  poincarejs \
)

# Post process poincare.d.ts
ifeq ($(EMIT_TSD),1)
define LD_WRAPPER_poincare
	$1
	./poincare/poincarejs/post_process_ts.sh $(OUTPUT_DIRECTORY)
endef
endif

$(OUTPUT_DIRECTORY)/poincare.js: poincare/poincarejs/pre-js.js poincare/poincarejs/extern-pre-js.js poincare/poincarejs/post_process_ts.sh

# Byproducts of building poincare.js
$(OUTPUT_DIRECTORY)/poincare.wasm: $(OUTPUT_DIRECTORY)/poincare.js
$(OUTPUT_DIRECTORY)/poincare.d.ts: $(OUTPUT_DIRECTORY)/poincare.js

# Global types
$(OUTPUT_DIRECTORY)/global.d.ts: poincare/poincarejs/global.d.ts
	cp poincare/poincarejs/global.d.ts $@

# Version file
.PHONY: $(OUTPUT_DIRECTORY)/.version.json
$(OUTPUT_DIRECTORY)/.version.json:
	@mkdir -p $(OUTPUT_DIRECTORY)
	@echo '{' > $@
	@echo '  "version": "$(APP_VERSION)",' >> $@
	@echo '  "commit": "$(PATCH_LEVEL)",' >> $@
	@echo '  "debug": "$(DEBUG)"' >> $@
	@echo '}' >> $@

# Documentation
$(OUTPUT_DIRECTORY)/README.md: poincare/poincarejs/docs.md
	cp poincare/poincarejs/docs.md $@

# Zip poincare.js and related files
ZIPPED_FILES := poincare.js poincare.wasm .version.json README.md
# Add .d.ts file if EMIT_TSD is set to 1
ifeq ($(EMIT_TSD),1)
    ZIPPED_FILES := $(ZIPPED_FILES) poincare.d.ts global.d.ts
endif

$(call create_zip,poincarejs.zip,$(addprefix $(OUTPUT_DIRECTORY)/,$(ZIPPED_FILES)))

$(call document_other_target,poincarejs.zip,Pack poincare.js and related files inside a zip ready to be copied in foxglove)

# --- poincare.js.run (tests) ---

# Node expects modules to end with .mjs
$(OUTPUT_DIRECTORY)/poincare.mjs: $(OUTPUT_DIRECTORY)/poincare.js
	cp $(OUTPUT_DIRECTORY)/poincare.js $(OUTPUT_DIRECTORY)/poincare.mjs

poincare.js.run: $(OUTPUT_DIRECTORY)/poincare.mjs
	cp poincare/test/poincarejs-test.mjs $(OUTPUT_DIRECTORY)
	cd $(OUTPUT_DIRECTORY)/ && node poincarejs-test.mjs

$(call document_other_target,poincare.js.run,Test poincare.js using node)
