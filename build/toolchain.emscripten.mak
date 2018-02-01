CC = emcc
CXX = emcc
LD = emcc
SIZE = size

SFLAGS = -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1
LDFLAGS =  -Oz -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EXPORTED_FUNCTIONS="['_main', '_IonEventsEmscriptenPushEvent']"
