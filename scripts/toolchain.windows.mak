CC = x86_64-w64-mingw32-gcc
CXX = x86_64-w64-mingw32-g++
LD = x86_64-w64-mingw32-g++
EXE = exe

SFLAGS += -D_USE_MATH_DEFINES
#LDFLAGS += -static -mwindows
LDFLAGS += -static

WINDRES = x86_64-w64-mingw32-windres
