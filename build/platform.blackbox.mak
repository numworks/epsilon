TOOLCHAIN ?= host-gcc
USE_LIBA ?= 0
EXE = bin
EPSILON_ONBOARDING_APP = 0

ifeq ($(DEBUG),1)
else
SFLAGS += -DNDEBUG
endif
