include build/toolchain.arm-gcc.mak
CC = clang
CXX = clang++
SFLAGS = -target thumbv7em-unknown-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16
