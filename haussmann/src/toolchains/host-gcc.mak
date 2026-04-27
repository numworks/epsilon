CC := gcc
CXX := g++
LD := g++
GDB := gdb

EXECUTABLE_EXTENSION := bin

# This library is used to obtain a backtrace using <stacktrace>
# when compiling with the fuzzer_triage flavor.
LDFLAGS += -lstdc++exp

COMPILER_FAMILY := gcc
