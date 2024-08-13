# TODO update when making poincare its own repository

# Assert this is being run from the repository's top level, as haussmann does
# not like having .. in paths.
ifeq ($(shell find . -maxdepth 1 -name '.git'),)
$(error For now, this makefile should be run from the root of epsilon. Use "make -f poincare/Makefile ..." instead)
endif

PATH_haussmann := haussmann
APP_NAME := Poincare
APP_VERSION := 0.0.0
OUTPUT_ROOT := output
DEBUG ?= 0
PLATFORM ?= foxglove
EMIT_TSD ?= 1
include $(PATH_haussmann)/Makefile

ASSERTIONS ?= $(DEBUG)

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
$(call import_module,escher,escher)
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
  storage/file_system.cpp \
  storage/record_name_verifier.cpp \
  storage/record.cpp \
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
  init.cpp \
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

# Make the module initialization synchronous
LDFLAGS_poincarejs += -s WASM_ASYNC_COMPILATION=0 \

# We remove 'node' from the environment because it is not working
# with the combination of WASM_ASYNC_COMPILATION=0 and EXPORT_ES6=1
# poincare.js can still be used with node though, probably because
# there is no behaviour like fetching data that require to differentiate
# the environments.
LDFLAGS_poincarejs += -s ENVIRONMENT='web,webview,worker' \

# Generate ts types file
# For this to work, go to emsdk/upstream/emscripten and run:
# > npm install typescript
# See https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#typescript-definitions
ifeq ($(EMIT_TSD),1)
LDFLAGS_poincarejs += --emit-tsd poincare.d.ts
endif

LDFLAGS_poincarejs += \
  -lembind \
  -s EXPORT_ES6=1 \

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_poincarejs)): $(KANDINSKY_fonts_dependencies)

# Rules

$(call create_goal,poincare, \
  escher \
  kandinsky \
  omg \
  poincare.js_bridge \
  poincarejs \
)

# Post process poincare.js and poincare.d.ts
define LD_WRAPPER_poincare
	$1
	./poincare/poincarejs/post_process_poincarejs.sh $(OUTPUT_DIRECTORY)
endef

$(OUTPUT_DIRECTORY)/poincare.js: poincare/poincarejs/post_process_poincarejs.sh poincare/poincarejs/poincare-partial.js

# Byproducts of building poincare.js
$(OUTPUT_DIRECTORY)/poincare.wasm: $(OUTPUT_DIRECTORY)/poincare.js
$(OUTPUT_DIRECTORY)/poincare.d.ts: $(OUTPUT_DIRECTORY)/poincare.js poincare/poincarejs/post_process_poincarejs.sh poincare/poincarejs/poincare-partial.d.ts

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
    ZIPPED_FILES := $(ZIPPED_FILES) poincare.d.ts
endif

$(call create_zip,poincarejs.zip,$(addprefix $(OUTPUT_DIRECTORY)/,$(ZIPPED_FILES)))

$(call document_other_target,poincarejs.zip,Pack poincare.js and related files inside a zip ready to be copied in foxglove)

# --- poincare.js.run (tests) ---

# Node expects modules to end with .mjs
$(OUTPUT_DIRECTORY)/poincare.mjs: $(OUTPUT_DIRECTORY)/poincare.js
	cp $(OUTPUT_DIRECTORY)/poincare.js $(OUTPUT_DIRECTORY)/poincare.mjs

poincare.js.run: $(OUTPUT_DIRECTORY)/poincare.mjs
	cp poincare/test/api-test.mjs $(OUTPUT_DIRECTORY)
	cd $(OUTPUT_DIRECTORY)/ && node api-test.mjs

$(call document_other_target,poincare.js.run,Test poincare.js using node)
