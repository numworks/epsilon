CC = emcc
CXX = emcc
LD = emcc
CPP = cpp

ifeq ($(DEBUG),1)
LDFLAGS += --profiling-funcs
LDFLAGS += -s ASSERTIONS=1
LDFLAGS += -s SAFE_HEAP=1
LDFLAGS += -s STACK_OVERFLOW_CHECK=1
endif

# Configure LDFLAGS
EMSCRIPTEN_MODULARIZE ?= 1
LDFLAGS += -s MODULARIZE=$(EMSCRIPTEN_MODULARIZE) -s 'EXPORT_NAME="Epsilon"'

LDFLAGS += -Oz -s SINGLE_FILE -s PRECISE_F32=1 -s ASYNCIFY=1 -s EXPORTED_FUNCTIONS='["_main", "_IonSimulatorKeyboardKeyDown", "_IonSimulatorKeyboardKeyUp", "_IonSimulatorEventsPushEvent", "_IonSoftwareVersion", "_IonPatchLevel"]' -s EXPORTED_RUNTIME_METHODS='["UTF8ToString"]'
