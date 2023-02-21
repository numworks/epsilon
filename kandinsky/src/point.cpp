#include <assert.h>
#include <kandinsky/point.h>

KDPoint KDPoint::translatedBy(KDPoint other) const {
  assert((other.x() >= 0 && m_x <= KDCOORDINATE_MAX - other.x()) ||
         (other.x() < 0 && m_x >= KDCOORDINATE_MIN - other.x()));
  assert((other.y() >= 0 && m_y <= KDCOORDINATE_MAX - other.y()) ||
         (other.y() < 0 && m_y >= KDCOORDINATE_MIN - other.y()));

  return KDPoint(m_x + other.x(), m_y + other.y());
}

KDPoint KDPoint::opposite() const { return KDPoint(-m_x, -m_y); }

uint16_t KDPoint::squareDistanceTo(KDPoint other) const {
  return (m_x - other.x()) * (m_x - other.x()) +
         (m_y - other.y()) * (m_y - other.y());
}
