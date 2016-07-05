#include <kandinsky/rect.h>
#include <kandinsky/pixel.h>
#include <kandinsky/context.h>
#include <string.h>
#include <assert.h>

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

static void KDRectComputeUnionBound(KDCoordinate size1, KDCoordinate size2,
    KDCoordinate * outputMin, KDCoordinate * outputMax,
    KDCoordinate min1, KDCoordinate min2,
    KDCoordinate max1, KDCoordinate max2)
{
  if (size1 != 0) {
    if (size2 != 0) {
      *outputMin = min(min1, min2);
      *outputMax = max(max1, max2);
    } else {
      *outputMin = min1;
      *outputMax = max1;
    }
  } else {
    if (size2 != 0) {
      *outputMin = min2;
      *outputMax = max2;
    }
  }
}

KDRect KDRectUnion(KDRect r1, KDRect r2) {
  /* We should ignore coordinate whose size is zero
   * For example, if r1.height is zero, just ignore r1.y and r1.height. */

  KDCoordinate resultLeft = 0;
  KDCoordinate resultTop = 0;
  KDCoordinate resultRight = 0;
  KDCoordinate resultBottom = 0;

  KDRectComputeUnionBound(r1.width, r2.width,
      &resultLeft, &resultRight,
      left(r1), left(r2),
      right(r1), right(r2));

  KDRectComputeUnionBound(r1.height, r2.height,
      &resultTop, &resultBottom,
      top(r1), top(r2),
      bottom(r1), bottom(r2));

  return (KDRect){
    .x = resultLeft,
    .y = resultTop,
    .width = resultRight-resultLeft,
    .height = resultBottom-resultTop
  };
}

bool KDRectContains(KDRect r, KDPoint p) {
  return (p.x >= r.x && p.x < (r.x+r.width) && p.y >= r.y && p.y < (r.y+r.height));
}

KDRect KDRectTranslate(KDRect r, KDPoint p) {
  return (KDRect){
    .origin = KDPointTranslate(r.origin, p),
    .size = r.size
  };
}

void KDPerPixelFillRect(KDCoordinate x, KDCoordinate y,
    KDCoordinate width, KDCoordinate height,
    KDColor * pattern, size_t patternSize) {
  size_t offset = 0;
  for (KDCoordinate j=0; j<height; j++) {
    for (KDCoordinate i=0; i<width; i++) {
      KDCurrentContext->setPixel(x+i, y+j, pattern[offset++]);
      if (offset >= patternSize) {
        offset = 0;
      }
    }
  }
}

void KDFillRect(KDRect rect, const KDColor * pattern, size_t patternSize) {
  assert(patternSize >= 1);
  KDRect absolutRect = rect;
  absolutRect.origin = KDPointTranslate(absolutRect.origin, KDCurrentContext->origin);

  KDRect rectToBeFilled = KDRectIntersection(absolutRect, KDCurrentContext->clippingRect);

  void (*fillRectFunction)(KDCoordinate x, KDCoordinate y,
      KDCoordinate width, KDCoordinate height,
      KDColor * pattern, size_t patternSize) = KDCurrentContext->fillRect;
  if (fillRectFunction == NULL) {
    fillRectFunction = KDPerPixelFillRect;
  }

  fillRectFunction(rectToBeFilled.x, rectToBeFilled.y,
      rectToBeFilled.width, rectToBeFilled.height,
      pattern, patternSize);
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
