CC = x86_64-w64-mingw32-gcc
CXX = x86_64-w64-mingw32-g++
LD = x86_64-w64-mingw32-g++
EXE = exe

SFLAGS += -D_USE_MATH_DEFINES
LDFLAGS += -static

ifeq ($(DEBUG),0)
# Defining "-mwindows" prevents the app from launching an instance of cmd.exe
# when running the app. That terminal will receive stderr and stdout. The
# downside of not defining "-mwindows" is that you lose stdout and stderr.
LDFLAGS += -mwindows
endif

WINDRES = x86_64-w64-mingw32-windres
