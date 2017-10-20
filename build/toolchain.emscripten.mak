CC = emcc
CXX = emcc
LD = emcc
LDFLAGS = -s EXPORTED_FUNCTIONS="['_main', '_IonEventsEmscriptenPushEvent']" -Os
SIZE = size
