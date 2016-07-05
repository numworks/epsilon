#ifndef KANDINSKY_CONTEXT_H
#define KANDINSKY_CONTEXT_H

#include "types.h"
#include "rect.h"

typedef struct {
  void (*setPixel)(KDCoordinate x, KDCoordinate y, KDColor color);
  void (*fillRect)(KDCoordinate x, KDCoordinate y,
      KDCoordinate width, KDCoordinate height,
      KDColor color);
  KDPoint origin;
  KDRect clippingRect;
} KDContext;

extern KDContext * KDCurrentContext;

#endif
