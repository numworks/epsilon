#ifndef KANDINSKY_RECT_H
#define KANDINSKY_RECT_H

#include <stdbool.h>
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

bool KDRectIntersect(KDRect r1, KDRect r2);
KDRect KDRectIntersection(KDRect r1, KDRect r2);

bool KDRectContains(KDRect r, KDPoint p);
KDRect KDRectTranslate(KDRect r, KDPoint p);

void KDFillRect(KDRect rect, KDColor color);
void KDDrawRect(KDRect rect, KDColor color);

#endif
