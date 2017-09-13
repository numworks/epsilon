#include <kandinsky/rect.h>

bool KDRect::intersects(const KDRect & other) const {
  return (
      other.right() >= left() &&
      other.left() < rightp1() &&
      other.top() < bottomp1() &&
      other.bottom() >= top()
      );
}

KDRect KDRect::intersectedWith(const KDRect & other) const {
  if (!intersects(other)) {
    return KDRectZero;
  }

  KDCoordinate intersectionLeft = max(left(), other.left());
  KDCoordinate intersectionRight = min(right(), other.right());
  KDCoordinate intersectionTop = max(top(), other.top());
  KDCoordinate intersectionBottom = min(bottom(), other.bottom());

  return KDRect(
      intersectionLeft,
      intersectionTop,
      intersectionRight-intersectionLeft+1,
      intersectionBottom-intersectionTop+1);
}

void computeUnionBound(KDCoordinate size1, KDCoordinate size2,
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

KDRect KDRect::unionedWith(const KDRect & other) const {
  if (this->isEmpty()) {
    return other;
  }
  if (other.isEmpty()) {
    return *this;
  }
  /* We should ignore coordinate whose size is zero
   * For example, if r1.height is zero, just ignore r1.y and r1.height. */

  KDCoordinate resultLeft = 0;
  KDCoordinate resultTop = 0;
  KDCoordinate resultRight = 0;
  KDCoordinate resultBottom = 0;

  computeUnionBound(width(), other.width(),
      &resultLeft, &resultRight,
      left(), other.left(),
      right(), other.right());

  computeUnionBound(height(), other.height(),
      &resultTop, &resultBottom,
      top(), other.top(),
      bottom(), other.bottom());

  return KDRect(
    resultLeft,
    resultTop,
    resultRight-resultLeft+1,
    resultBottom-resultTop+1
    );
}

KDRect KDRect::differencedWith(const KDRect & other) const {
  if (this->isEmpty() || other.isEmpty()) {
    return *this;
  }

  KDRect intersection = intersectedWith(other);
  if (intersection.isEmpty()) {
    return *this;
  }

  if (intersection == *this) {
    return KDRectZero;
  }

  KDCoordinate resultLeft = left();
  KDCoordinate resultTop = top();
  KDCoordinate resultRight = right();
  KDCoordinate resultBottom = bottom();

  if (intersection.height() == height()) {
    if (intersection.left() == left()) {
      resultLeft = intersection.right() + 1;
    } else if (intersection.right() == right()) {
      resultRight = intersection.left() - 1;
    }
  } else if (intersection.width() == width()) {
    if (intersection.top() == top()) {
      resultTop = intersection.bottom() + 1;
    } else if (intersection.bottom() == bottom()) {
      resultBottom = intersection.top() - 1;
    }
  }

  return KDRect(
    resultLeft,
    resultTop,
    resultRight-resultLeft+1,
    resultBottom-resultTop+1
    );
}

bool KDRect::contains(KDPoint p) const {
  return (p.x() >= x() && p.x() <= right() && p.y() >= y() && p.y() <= bottom());
}
