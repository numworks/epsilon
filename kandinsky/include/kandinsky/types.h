#ifndef KANDINSKY_TYPES_H
#define KANDINSKY_TYPES_H

#include <stdint.h>

typedef uint16_t KDCoordinate;

typedef struct {
  KDCoordinate x;
  KDCoordinate y;
} KDPoint;

typedef struct {
  KDCoordinate width;
  KDCoordinate height;
} KDSize;

#endif
