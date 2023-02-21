#include <assert.h>
#include <kandinsky/context.h>
#include <stdlib.h>

#include <algorithm>
#include <cmath>

void KDContext::drawLine(KDPoint p1, KDPoint p2, KDColor c) {
  // Find the largest gap
  KDPoint left = KDPointZero, right = KDPointZero;
  if (p2.x() > p1.x()) {
    left = p1;
    right = p2;
  } else {
    left = p2;
    right = p1;
  }
  KDPoint top = KDPointZero, bottom = KDPointZero;
  if (p2.y() > p1.y()) {
    top = p1;
    bottom = p2;
  } else {
    top = p2;
    bottom = p1;
  }
  assert(right.x() >= left.x());
  assert(bottom.y() >= top.y());

  KDCoordinate deltaX = 2 * (right.x() - left.x());
  KDCoordinate deltaY = 2 * (bottom.y() - top.y());

  KDPoint p = KDPointZero, alwaysTranslate = KDPointZero,
          conditionalTranslate = KDPointZero;
  KDCoordinate scanLength, error, minusError, plusError;

  if (deltaX >= deltaY) {
    p = left;
    scanLength = right.x() - left.x();
    error = right.x() - left.x();
    minusError = deltaY;
    plusError = deltaX;
    alwaysTranslate = KDPoint(1, 0);
    conditionalTranslate = KDPoint(0, (right.y() >= left.y() ? 1 : -1));
  } else {
    p = top;
    scanLength = bottom.y() - top.y();
    error = bottom.y() - top.y();
    minusError = deltaX;
    plusError = deltaY;
    alwaysTranslate = KDPoint(0, 1);
    conditionalTranslate = KDPoint((bottom.x() >= top.x() ? 1 : -1), 0);
  }

  KDCoordinate scanCounter = 0;
  while (scanCounter++ < scanLength) {
    setPixel(p, c);
    p = p.translatedBy(alwaysTranslate);
    error = error - minusError;
    if (error <= 0) {
      p = p.translatedBy(conditionalTranslate);
      error = error + plusError;
    }
  }
}

void KDContext::drawAntialiasedLine(float x1, float y1, float x2, float y2,
                                    KDColor c, KDColor background) {
  // Implements Xiaolin Wu's line algorithm
  // https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm

  bool steep = abs(y2 - y1) > abs(x2 - x1);
  if (steep) {
    std::swap(x1, y1);
    std::swap(x2, y2);
  }
  if (x1 > x2) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  float dx = x2 - x1;
  float dy = y2 - y1;
  double gradient = (dx == 0) ? 1 : static_cast<double>(dy) / dx;

  for (int x = x1; x <= x2; x++) {
    double y = y1 + gradient * (x - x1);
    int yBelow = std::floor(y);
    int yAbove = yBelow + 1;
    float fractionalPart = y - yBelow;
    uint8_t alpha = 255u * (1 - fractionalPart);
    KDColor colorBelow = KDColor::Blend(c, background, alpha);
    KDColor colorAbove = KDColor::Blend(c, background, 255 - alpha);
    if (steep) {
      setPixel(KDPoint(yBelow, x), colorBelow);
      setPixel(KDPoint(yAbove, x), colorAbove);
    } else {
      setPixel(KDPoint(x, yBelow), colorBelow);
      setPixel(KDPoint(x, yAbove), colorAbove);
    }
  }
}
