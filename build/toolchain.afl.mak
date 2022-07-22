export AFL_QUIET = 1
CC = afl-cc
CXX = afl-c++
LD = afl-c++

ifeq ($(ASAN),1)
export AFL_USE_ASAN = 1
endif

# Prevent NDEBUG from being defined since we will always want assertions to run
# when the code is being fuzzed
SFLAGS := $(filter-out -DNDEBUG,$(SFLAGS))

ifeq ($(DEBUG),1)
$(error DEBUG=1 is unnecessary with afl toolchain: assertions are handled anyway but the build won't be optimized.)
endif
