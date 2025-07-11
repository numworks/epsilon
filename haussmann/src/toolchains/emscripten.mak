EMSCRIPTEN_MODULARIZE ?= 1

# Read required version from .emsdk-version file
EMCC_REQUIRED_VERSION := $(shell cat .emsdk-version)

CC := emcc
CXX := emcc
AR := emar
LD := emcc

EXECUTABLE_EXTENSION := js

COMPILER_FAMILY := clang

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
# Optimization level:
# -O0: No optimization
# -O3: Maximum optimization
# -Os: Optimize for size
# -Oz: Optimize aggressively for size
#
# -Oz: Not working
# For some reason builds with Oz crash when dlopening external apps
# The build with Os is barely larger than with Oz : +0.5% on epsilon.js
#
# -O3 or -Os ?
# Build size comparison:
# Optim | poincare.wasm | epsilon.js
#  -O0  |    637 KB     | 10 375 KB
#  -O3  |    586 KB     |  5 628 KB
#  -Os  |    555 KB     |  5 533 KB
# -O3 -> -Os: -5.3% size on poincare.wasm, -1.7% on epsilon.js
#
# Computation speed comparison
# Test: Repeat 10 000 times parse, reduce, approximate and beautify on the
# latex string "((π/3)+(π/6)+(π/4))/(sqrt(40))"
# Optim | poincare.wasm 
#  -O0  |  ~ 9.3s
#  -O3  |  ~ 6.0 s
#  -Os  |  ~ 7.2 s
# -O3 -> -Os: +20% time
#
# We'll go for -O3 as it's significantly faster and almost as small as -Os
# Also a few KB don't matter much in the context of web apps, while speed does
LDFLAGS += -O3
endif
