#include <kandinsky/rect.h>
#include <kandinsky/pixel.h>
#include <string.h>

KDRect KDRectZero = {.x = 0, .y = 0, .width = 0, .height = 0};

KDRect KDRectIntersect(KDRect r1, KDRect r2) {
  KDRect intersection;

  // Let's start by computing the overlap on the X axis
  if (r1.x < r2.x) {
    intersection.x = r2.x;
    intersection.width = r1.x+r1.width-r2.x;
  } else {
    intersection.x = r1.x;
    intersection.width = r2.x+r2.width-r1.x;
  }

  if (intersection.width < 0) {
    // There's no overlap on the X axis, let's bail out
    return KDRectZero;
  }

  // Let's then compute the overlap on the Y axis
  if (r1.y < r2.y) {
    intersection.y = r2.y;
    intersection.height = r1.y+r1.height-r2.y;
  } else {
    intersection.y = r1.y;
    intersection.height = r2.y+r2.height-r1.y;
  }

  if (intersection.height < 0) {
    // There's no overlap on the Y axis, let's bail out
    return KDRectZero;
  }

  return intersection;
}

void KDFillRect(KDRect rect, KDColor color) {
  KDPoint p;
  for (p.x = rect.x; p.x<(rect.x+rect.width); p.x++) {
    for (p.y = rect.y; p.y<(rect.y+rect.height); p.y++) {
      KDSetPixel(p, color);
    }
  }
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
