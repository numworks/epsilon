#ifndef KANDINSKY_TYPES_H
#define KANDINSKY_TYPES_H

#include <stdint.h>

typedef int16_t KDCoordinate;
#define KDCoordinateMax ((KDCoordinate)((1<<15)-1))

typedef struct {
  KDCoordinate x;
  KDCoordinate y;
} KDPoint;

extern KDPoint KDPointZero;

static inline KDPoint KDPointMake(KDCoordinate x, KDCoordinate y) {
  KDPoint p;
  p.x = x;
  p.y = y;
  return p;
}

static inline KDPoint KDPointTranslate(KDPoint p1, KDPoint p2) {
  KDPoint p;
  p.x = p1.x+p2.x;
  p.y = p1.y+p2.y;
  return p;
}

typedef struct {
  KDCoordinate width;
  KDCoordinate height;
} KDSize;

#endif
