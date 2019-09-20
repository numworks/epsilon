ifeq ($(OS),Windows_NT)
MINGW_TOOLCHAIN_PREFIX=
else
MINGW_TOOLCHAIN_PREFIX=x86_64-w64-mingw32-
endif

CC = $(MINGW_TOOLCHAIN_PREFIX)gcc
CXX = $(MINGW_TOOLCHAIN_PREFIX)g++
LD = $(MINGW_TOOLCHAIN_PREFIX)g++
WINDRES = $(MINGW_TOOLCHAIN_PREFIX)windres

SFLAGS += -D_USE_MATH_DEFINES
LDFLAGS += -static

ifeq ($(DEBUG),0)
# Defining "-mwindows" prevents the app from launching an instance of cmd.exe
# when running the app. That terminal will receive stderr and stdout. The
# downside of not defining "-mwindows" is that you lose stdout and stderr.
LDFLAGS += -mwindows
endif
