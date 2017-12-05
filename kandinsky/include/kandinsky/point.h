#ifndef KANDINSKY_POINT_H
#define KANDINSKY_POINT_H

#include <kandinsky/coordinate.h>

class KDPoint final {
public:
  constexpr KDPoint(KDCoordinate x, KDCoordinate y) :
    m_x(x), m_y(y) {}
  constexpr KDCoordinate x() const { return m_x; }
  constexpr KDCoordinate y() const { return m_y; }
  constexpr KDPoint translatedBy(KDPoint other) const {
    return KDPoint(m_x+other.x(), m_y+other.y());
  }
  constexpr KDPoint opposite() const {
    return KDPoint(-m_x, -m_y);
  }
  bool operator ==(const KDPoint &other) const {
    return (m_x == other.m_x && m_y == other.m_y);
  }
  bool operator !=(const KDPoint &other) const {
    return !(operator ==(other));
  }
  constexpr uint16_t squareDistanceTo(KDPoint other) const {
    return (m_x-other.x()) * (m_x-other.x()) + (m_y-other.y()) * (m_y-other.y());
  }
private:
  KDCoordinate m_x;
  KDCoordinate m_y;
};

constexpr KDPoint KDPointZero = KDPoint(0,0);

#endif
