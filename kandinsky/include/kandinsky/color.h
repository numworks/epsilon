#ifndef KANDINSKY_COLOR_H
#define KANDINSKY_COLOR_H

#include <stdint.h>

// Kandinsky uses RGB565

typedef uint16_t KDColor;

/* KDColorRGB takes a 32-bits RGB color and builds a KDColor from it.
 * Given KDColor is RGB565 and that we take 8-bit values for R, G, and B,
 * some shifting has to happen. */

#define KDColorRGB(r,g,b) ((KDColor)((((((uint16_t)(r))>>3)&0x1F)<<11)|(((((uint16_t)(g))>>2)&0x3F)<<5)|((((uint16_t)(b))>>3)&0x1F)))

#define KDColorBlack KDColorRGB(0x00, 0x00, 0x00)
#define KDColorWhite KDColorRGB(0xFF, 0xFF, 0xFF)

#define KDColorRed KDColorRGB(0xFF, 0x00, 0x00)
#define KDColorGreen KDColorRGB(0x00, 0xFF, 0x00)
#define KDColorBlue KDColorRGB(0x00, 0x00, 0xFF)

#endif
