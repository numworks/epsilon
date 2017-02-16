CC = emcc
CXX = emcc
LD = emcc
LDFLAGS = -s EXPORTED_FUNCTIONS="['_main', '_IonEmscriptenPushEvent']" -Os --shell-file ion/src/emscripten/shell.html
SIZE = size
