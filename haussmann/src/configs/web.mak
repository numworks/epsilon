TOOLCHAIN ?= emscripten
_emscripten_asyncify := 1
_emscripten_single_file := 1
# -O3 breaks since emsdk 4.0.23 because it strips some asyncify symbols
# e.g. asyncify_get_state that cannot be whitelisted
_emscripten_binaryen_opt := -O2
