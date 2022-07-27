export AFL_QUIET = 1
CC = afl-cc
CXX = afl-c++
LD = afl-c++

ifeq ($(ASAN),1)
export AFL_USE_ASAN = 1
endif

# Always use assertions when the code is being fuzzed
ASSERTIONS = 1

ifeq ($(ASSERTIONS),0)
$(error ASSERTIONS=0 is useless with afl toolchain: assertions won't be handled.)
endif

ifeq ($(DEBUG),1)
$(error DEBUG=1 is unnecessary with afl toolchain: assertions are handled anyway but the build won't be optimized.)
endif
