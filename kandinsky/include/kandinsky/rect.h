#ifndef KANDINSKY_RECT_H
#define KANDINSKY_RECT_H

#include <kandinsky/color.h>
#include <kandinsky/types.h>

typedef struct {
  union {
    struct {
      KDCoordinate x;
      KDCoordinate y;
    };
    KDPoint origin;
  };
  union {
    struct {
      KDCoordinate width;
      KDCoordinate height;
    };
    KDSize size;
  };
} KDRect;

extern KDRect KDRectZero;

void KDFillRect(KDRect rect, KDColor color);

#endif
