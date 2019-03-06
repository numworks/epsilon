TOOLCHAIN ?= android
EXE = so

LDFLAGS += -L$(SDL_PATH)/build/android/lib/$(NDK_ABI)
LDFLAGS += -lSDL2 -ljnigraphics -llog
