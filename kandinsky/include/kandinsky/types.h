#ifndef KANDINSKY_TYPES_H
#define KANDINSKY_TYPES_H

#include <stdint.h>

typedef uint16_t KDCoordinate;

typedef struct {
  KDCoordinate x;
  KDCoordinate y;
} KDPoint;

#define KDPOINT(xc,yc) ((KDPoint){.x=(KDCoordinate)(xc),.y=(KDCoordinate)(yc)})

typedef struct {
  KDCoordinate width;
  KDCoordinate height;
} KDSize;

#endif
