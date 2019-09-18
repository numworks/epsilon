TOOLCHAIN ?= host-gcc
ifeq ($(OS),Windows_NT)
  TOOLCHAIN = mingw
endif
USE_LIBA = 0
ION_KEYBOARD_LAYOUT = layout_B2
EXE = elf
EPSILON_ONBOARDING_APP = 0
SFLAGS += -fPIE
