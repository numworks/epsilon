#ifndef LIBA_IEEE754_H
#define LIBA_IEEE754_H

#include <stdint.h>

uint32_t ieee754man32(float x);
uint8_t ieee754exp32(float x);

uint64_t ieee754man64(double x);
uint16_t ieee754exp64(double x);

#endif
