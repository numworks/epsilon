EMSCRIPTEN_MODULARIZE ?= 1

CC := emcc
CXX := emcc
AR := emar
LD := emcc

EXECUTABLE_EXTENSION := js

# Modules should add EXPORTED_FUNCTIONS and EXPORTED_RUNTIME_METHODS to LDFLAGS.
LDFLAGS += \
  -s ASYNCIFY=$(_emscripten_asyncify) \
  -s EXPORT_NAME="$(APP_NAME)" \
  -s MODULARIZE=$(EMSCRIPTEN_MODULARIZE) \

ifeq ($(_emscripten_single_file),1)
LDFLAGS += -s SINGLE_FILE
endif

# See emscripten compiler settings documentation
# https://emscripten.org/docs/tools_reference/settings_reference.html
ifeq ($(ASSERTIONS),1)
LDFLAGS += \
  -s ASSERTIONS=1 \
  -s SAFE_HEAP=1 \
  -s STACK_OVERFLOW_CHECK=1
endif

ifeq ($(DEBUG),1)
LDFLAGS += \
  -O0 \
  --profiling-funcs
else
# TODO: Should this be:
# - Oz ? (small size, slower)
# - O3 ? (large size, faster)
# - Os ? (in between)
# For some reason builds with Oz crash when dlopening external apps
LDFLAGS += -Os
endif
