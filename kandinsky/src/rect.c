#include <kandinsky/rect.h>
#include <kandinsky/pixel.h>
#include <string.h>
#include "private/drawing_area.h"
#include <ion.h>

KDRect KDRectZero = {.x = 0, .y = 0, .width = 0, .height = 0};

static inline KDCoordinate left(KDRect r) { return r.x; }
static inline KDCoordinate right(KDRect r) { return r.x+r.width; }
static inline KDCoordinate top(KDRect r) { return r.y; }
static inline KDCoordinate bottom(KDRect r) { return r.y+r.height; }
static inline KDCoordinate min(KDCoordinate x, KDCoordinate y) { return (x<y ? x : y); }
static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return (x>y ? x : y); }

bool KDRectIntersect(KDRect r1, KDRect r2) {
    return !(right(r2) < left(r1)
             ||
             left(r2) > right(r1)
             ||
             top(r2) > bottom(r1)
             ||
             bottom(r2) < top(r1)
             );
}

KDRect KDRectIntersection(KDRect r1, KDRect r2) {
    if (!KDRectIntersect(r1, r2)) {
        return KDRectZero;
    }

    KDCoordinate intersectionLeft = max(left(r1), left(r2));
    KDCoordinate intersectionRight = min(right(r1), right(r2));
    KDCoordinate intersectionTop = max(top(r1), top(r2));
    KDCoordinate intersectionBottom = min(bottom(r1), bottom(r2));

    KDRect intersection;
    intersection.x = intersectionLeft;
    intersection.width = intersectionRight-intersectionLeft;
    intersection.y = intersectionTop;
    intersection.height = intersectionBottom-intersectionTop;
    return intersection;
}

void KDFillRect(KDRect rect, KDColor color) {
  KDRect rectToBeFilled = {
    .x = rect.x + KDDrawingArea.x,
    .y = rect.y + KDDrawingArea.y,
    .width = min(rect.width, KDDrawingArea.width),
    .height = min(rect.height, KDDrawingArea.height)
  };
  ion_fill_rect(rectToBeFilled.x, rectToBeFilled.y,
      rectToBeFilled.width, rectToBeFilled.height,
      color);
}

void KDDrawRect(KDRect rect, KDColor color) {
  KDPoint p1, p2;
  p1.x = rect.x;
  p1.y = rect.y;
  p2.x = rect.x;
  p2.y = rect.y + rect.height;
  for (int i = 0; i<rect.width; i++) {
    KDSetPixel(p1, color);
    KDSetPixel(p2, color);
    p1.x++;
    p2.x++;
  }
  p1.x = rect.x;
  p1.y = rect.y;
  p2.x = rect.x + rect.width;
  p2.y = rect.y;
  for (int i = 0; i<rect.height; i++) {
    KDSetPixel(p1, color);
    KDSetPixel(p2, color);
    p1.y++;
    p2.y++;
  }
}
