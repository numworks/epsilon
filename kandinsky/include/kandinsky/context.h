#ifndef KANDINSKY_CONTEXT_H
#define KANDINSKY_CONTEXT_H

#include "types.h"
#include "rect.h"

typedef struct {
  void (*setPixel)(KDCoordinate x, KDCoordinate y, KDColor color);
  // fillRect can be left NULL.
  // In that case, Kandinsky will fall back to using setPixel only
  void (*fillRect)(KDCoordinate x, KDCoordinate y,
      KDCoordinate width, KDCoordinate height,
      KDColor * pattern, size_t patternSize);
  KDPoint origin;
  KDRect clippingRect;
} KDContext;

extern KDContext * KDCurrentContext;

#endif
