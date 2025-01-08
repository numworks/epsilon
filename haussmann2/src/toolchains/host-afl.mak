CC := afl-clang-lto
CXX := afl-clang-lto++
AR := ar
LD := afl-clang-lto++

EXECUTABLE_EXTENSION := bin

ifeq ($(ASSERTIONS),0)
# TODO: ASSERTIONS is set and used earlier in config.mak and cannot be overridden here.
$(error host-afl toolchain must be used with ASSERTIONS=1)
endif
