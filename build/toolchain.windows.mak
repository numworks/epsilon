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

# mingw doesn't support -rdynamic
LD_EXPORT_SYMBOLS_FLAG = -Wl,--export-all-symbols

# Generate an implib for executables
# This is required to build plugins (for example, NWB files) that can link
# against functions defined in the simulator.
# See https://stackoverflow.com/questions/39759060/compile-to-dll-with-some-undefined-references-with-mingw
$(BUILD_DIR)/%.$(EXE): LDFLAGS += -Wl,--out-implib,$(BUILD_DIR)/lib$*.a

ifeq ($(DEBUG),0)
# Defining "-mwindows" prevents the app from launching an instance of cmd.exe
# when running the app. That terminal will receive stderr and stdout. The
# downside of not defining "-mwindows" is that you lose stdout and stderr.
LDFLAGS += -mwindows
endif
