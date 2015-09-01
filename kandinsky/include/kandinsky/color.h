#ifndef KANDINSKY_COLOR_H
#define KANDINSKY_COLOR_H

#include <stdint.h>
#include <kandinsky/config.h>

#if (KD_BITS_PER_PIXEL & (KD_BITS_PER_PIXEL - 1))
#error KD_BITS_PER_PIXEL should be a power of two!
// PoT implies that all pixels are byte-aligned
#endif
#if KD_BITS_PER_PIXEL <= 8
typedef uint8_t KDColor;
#elif KD_BITS_PER_PIXEL == 16
typedef uint16_t KDColor;
#elif KD_BITS_PER_PIXEL == 32
typedef uint32_t KDColor;
#elif KD_BITS_PER_PIXEL == 64
typedef uint64_t KDColor;
#else
#error KD_BITS_PER_PIXEL is too large!
#endif

#endif
