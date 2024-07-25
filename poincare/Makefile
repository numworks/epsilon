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
include $(PATH_haussmann)/Makefile

ASSERTIONS ?= $(DEBUG)

SFLAGS += \
  -DASSERTIONS=$(ASSERTIONS)

# Modules

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
  layout_events.cpp \
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
LDFLAGS_poincarejs += --emit-tsd poincare.d.ts

LDFLAGS_poincarejs += \
  -lembind \
  -s EXPORT_ES6=1 \

# Rules

$(call create_goal,poincare, \
  escher \
  kandinsky \
  omg \
  poincare.js_bridge \
  poincarejs \
)

$(OUTPUT_DIRECTORY)/.version.json:
	@echo '{' > $@
	@echo '  "version": "$(APP_VERSION)",' >> $@
	@echo '  "commit": "$(PATCH_LEVEL)",' >> $@
	@echo '  "debug": "$(DEBUG)"' >> $@
	@echo '}' >> $@


# postprocess_poincare_js should be called after building poincare.js. It:
# - Replaces "new URL("poincare.wasm",import.meta.url).href" by "" in poincare.js
#    > This bit of code is never reached and makes cypress fail because the URL can't be resolved
#    > NOTE; There is a .* inside the regexp because in DEBUG=1 there is a space
# - Removes 2 errors thrown when building poincare.mjs in DEBUG=1
#    > This is needed because we removed 'node' from the ENVIRONMENT.
#    > See comment about ENVIRONMENT above.
# - Renames the "MainModule" type to "PoincareInstance".
#
# WARNING: This is a bit hacky, especially the part where the errors are removed.
postprocess_poincare_js: $(OUTPUT_DIRECTORY)/poincare.js
	sed -i '' -e 's/new URL("poincare.wasm",.*import\.meta\.url)\.href/""/g' \
		-e 's/throw new Error("environment detection error");//g' \
		-e 's/assert(!ENVIRONMENT_IS_NODE, "node environment detected but not enabled at build time.  Add `node` to `-sENVIRONMENT` to enable.");//g' $(OUTPUT_DIRECTORY)/poincare.js
	sed -i '' 's/MainModule/PoincareInstance/g' $(OUTPUT_DIRECTORY)/poincare.d.ts

# TODO: Restore this. Not working because of postprocess_poincare_js
# $(call create_zip,poincarejs.zip,$(addprefix $(OUTPUT_DIRECTORY)/,poincare.js poincare.d.ts poincare.wasm .version.json) postprocess_poincare_js)

# NOTE: Restore when "call create_zip" is restored
# Allows poincarejs.zip to know that these depend on poincare.js
# $(OUTPUT_DIRECTORY)/poincare.wasm: $(OUTPUT_DIRECTORY)/poincare.js
# $(OUTPUT_DIRECTORY)/poincare.d.ts: $(OUTPUT_DIRECTORY)/poincare.js

# This replaces "call create_zip"
$(OUTPUT_DIRECTORY)/poincarejs.zip: $(OUTPUT_DIRECTORY)/poincare.js $(OUTPUT_DIRECTORY)/.version.json postprocess_poincare_js
	cd $(OUTPUT_DIRECTORY) && \
	rm -rf poincarejs && \
	mkdir poincarejs && \
	cp poincare.js poincare.d.ts poincare.wasm .version.json poincarejs/ && \
	zip -r -9 -j poincarejs.zip poincarejs > /dev/null && \
	rm -rf poincarejs

$(call document_other_target,poincarejs.zip,Pack poincare.js and related files inside a zip ready to be copied in foxglove)

# Node expects modules to end with .mjs
$(OUTPUT_DIRECTORY)/poincare.mjs: $(OUTPUT_DIRECTORY)/poincare.js postprocess_poincare_js
	cp $(OUTPUT_DIRECTORY)/poincare.js $(OUTPUT_DIRECTORY)/poincare.mjs

poincare.js.run: $(OUTPUT_DIRECTORY)/poincare.mjs
	cp poincare/test/api-test.mjs $(OUTPUT_DIRECTORY)
	cd $(OUTPUT_DIRECTORY)/ && node api-test.mjs

$(call document_other_target,poincare.js.run,Test poincare.js using node)

$(call assert_defined,KANDINSKY_fonts_dependencies)
$(call all_objects_for,$(SOURCES_poincarejs)): $(KANDINSKY_fonts_dependencies)
