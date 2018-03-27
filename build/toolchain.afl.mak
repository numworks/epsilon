export AFL_QUIET = 1
CC = afl-clang
CXX = afl-clang++
LD = afl-clang++

ifeq ($(ASAN),1)
export AFL_USE_ASAN = 1
endif
