CC := clang
CXX := clang++
AR := ar
LD := clang++
GDB := lldb

EXECUTABLE_EXTENSION := bin

COMPILER_FAMILY := clang

LDFLAGS += -lc++
CXXFLAGS += -stdlib=libc++
SFLAGS += -Wno-unknown-warning-option
