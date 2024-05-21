EMSCRIPTEN_MODULARIZE ?= 1

CC := emcc
CXX := emcc
AR := emar
LD := emcc

EXECUTABLE_EXTENSION := js

# Modules should add EXPORTED_FUNCTIONS and EXPORTED_RUNTIME_METHODS to LDFLAGS.
LDFLAGS += \
  --memory-init-file 0 \
  -Oz \
  -s ASYNCIFY=1 \
  -s EXPORT_NAME="Epsilon" \
  -s MODULARIZE=$(EMSCRIPTEN_MODULARIZE) \
  -s PRECISE_F32=1 \
  -s WASM=0

ifneq ($(DEBUG),0)
LDFLAGS += \
  --profiling-funcs \
  -s ASSERTIONS=1 \
  -s SAFE_HEAP=1 \
  -s STACK_OVERFLOW_CHECK=1
endif

