#include <assert.h>
#include <kandinsky/line.h>
#include <kandinsky/pixel.h>

void KDDrawLine(KDPoint p1, KDPoint p2, KDColor c) {
  // Find the largest gap
  KDPoint left, right;
  if (p2.x > p1.x) {
    left = p1;
    right = p2;
  } else {
    left = p2;
    right = p1;
  }
  KDPoint top, bottom;
  if (p2.y > p1.y) {
    top = p1;
    bottom = p2;
  } else {
    top = p2;
    bottom = p1;
  }
  assert(right.x >= left.x);
  assert(bottom.y >= top.y);

  KDCoordinate deltaX = 2 * (right.x - left.x);
  KDCoordinate deltaY = 2 * (bottom.y - top.y);

  KDPoint p, alwaysTranslate, conditionalTranslate;
  KDCoordinate scanLength, error, minusError, plusError;

  if (deltaX >= deltaY) {
    p = left;
    scanLength = right.x - left.x;
    error = right.x - left.x;
    minusError = deltaY;
    plusError = deltaX;
    alwaysTranslate = KDPointMake(1, 0);
    conditionalTranslate = KDPointMake(0, (right.y >= left.y ? 1 : -1));
  } else {
    p = top;
    scanLength = bottom.y - top.y;
    error = bottom.y - top.y;
    minusError = deltaX;
    plusError = deltaY;
    alwaysTranslate = KDPointMake(0, 1);
    conditionalTranslate = KDPointMake((bottom.x >= top.x ? 1 : -1), 0);
  }

  KDCoordinate scanCounter = 0;
  while (scanCounter++ < scanLength) {
    KDSetPixel(p, c);
    p = KDPointTranslate(p, alwaysTranslate);
    error = error - minusError;
    if (error <= 0) {
      p = KDPointTranslate(p, conditionalTranslate);
      error = error + plusError;
    }
  }
}
/*
#include <stdlib.h>

#include <cmath>

void KDDrawAntiAliasedLine(KDPoint p1, KDPoint p2, KDCoordinate width, KDColor
frontColor, KDColor backColor) { int x0 = p1.x; int y0 = p1.y; int x1 = p2.x;
  int y1 = p2.y;
  float wd = width;

  int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
  int err = dx-dy, e2, x2, y2; // error value e_xy
  float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

  for (wd = (wd+1)/2; ; ) { // pixel loop
    KDPoint p = {.x = x0, .y = y0};
    KDColor color = KDColorMix(KDColorRed, KDColorWhite,
(abs(err-dx+dy)/ed-wd+1)); KDSetPixel(p, color); e2 = err; x2 = x0; if (2*e2 >=
-dx) { // x step for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2
+= dx) { y2 += sy; p = {.x = x0, .y = y2}; color = KDColorMix(KDColorRed,
KDColorWhite, (abs(err-dx+dy)/ed-wd+1)); setPixelColor(x0, y2 += sy,
max(0,255*(abs(e2)/ed-wd+1)));
      }
      if (x0 == x1) break;
      e2 = err; err -= dy; x0 += sx;
    }
    if (2*e2 <= dy) { // y step
      for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
        setPixelColor(x2 += sx, y0, max(0,255*(abs(e2)/ed-wd+1)));
      if (y0 == y1) break;
      err += dx; y0 += sy;
    }
  }
}
*/
