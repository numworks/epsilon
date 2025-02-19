CC := clang
CXX := clang++
AR := ar
LD := clang++
GDB := lldb

EXECUTABLE_EXTENSION := bin

LDFLAGS += -lc++
CXXFLAGS += -stdlib=libc++
