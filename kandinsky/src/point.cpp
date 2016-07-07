#include <kandinsky/point.h>

KDCoordinate KDPoint::x() const { return m_x; }
KDCoordinate KDPoint::y() const { return m_y; }

KDPoint KDPoint::translatedBy(KDPoint other) const {
  return KDPoint(m_x+other.x(), m_y+other.y());
}

KDPoint KDPoint::opposite() const {
  return KDPoint(-m_x, -m_y);
}
