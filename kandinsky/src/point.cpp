#include <kandinsky/point.h>

KDPoint KDPoint::translatedBy(KDPoint other) const {
  return KDPoint(m_x+other.x(), m_y+other.y());
}

KDPoint KDPoint::opposite() const {
  return KDPoint(-m_x, -m_y);
}
