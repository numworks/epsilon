#ifndef KANDINSKY_COLOR_H
#define KANDINSKY_COLOR_H

#include <stdint.h>

typedef uint16_t KDColor;

#define KDColorRGB(r,g,b) ((KDColor)(((r&0x1F)<<11)&((g&0x3F)<<5)&(b&0x1F)))

#endif
