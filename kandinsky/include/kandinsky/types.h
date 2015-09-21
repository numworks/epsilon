#ifndef KANDINSKY_TYPES_H
#define KANDINSKY_TYPES_H

#include <stdint.h>

typedef uint16_t KDCoordinate;

typedef struct {
  KDCoordinate x;
  KDCoordinate y;
} KDPoint;

#define KDPOINT(xc,yc) ((KDPoint){.x=(KDCoordinate)(xc),.y=(KDCoordinate)(yc)})
static inline KDPoint KDPointTranslate(KDPoint p1, KDPoint p2) {
  return (KDPoint){.x = (KDCoordinate)(p1.x + p2.x), .y = (KDCoordinate)(p1.y + p2.y)};
}

typedef struct {
  KDCoordinate width;
  KDCoordinate height;
} KDSize;

#endif
