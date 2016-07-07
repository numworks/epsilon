#include <kandinsky/context.h>
#include <assert.h>

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

  KDCoordinate deltaX = 2*(right.x() - left.x());
  KDCoordinate deltaY = 2*(bottom.y() - top.y());

  KDPoint p = KDPointZero, alwaysTranslate = KDPointZero, conditionalTranslate = KDPointZero;
  KDCoordinate scanLength, error, minusError, plusError;

  if (deltaX >= deltaY) {
    p = left;
    scanLength = right.x() - left.x();
    error = right.x() - left.x();
    minusError = deltaY;
    plusError = deltaX;
    alwaysTranslate = KDPoint(1,0);
    conditionalTranslate = KDPoint(0, (right.y() >= left.y() ? 1 : -1));
  } else {
    p = top;
    scanLength = bottom.y() - top.y();
    error = bottom.y() - top.y();
    minusError = deltaX;
    plusError = deltaY;
    alwaysTranslate = KDPoint(0,1);
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
