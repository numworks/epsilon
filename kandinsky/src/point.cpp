#include <assert.h>
#include <kandinsky/point.h>

KDPoint KDPoint::translatedBy(KDPoint other) const {
  return KDPoint(x() + other.x(), y() + other.y());
}

KDPoint KDPoint::opposite() const { return KDPoint(-x(), -y()); }

uint16_t KDPoint::squareDistanceTo(KDPoint other) const {
  return (x() - other.x()) * (x() - other.x()) +
         (y() - other.y()) * (y() - other.y());
}
