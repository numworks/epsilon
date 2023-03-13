#ifndef KANDINSKY_POINT_H
#define KANDINSKY_POINT_H

#include <kandinsky/coordinate.h>

class KDPoint {
 public:
  constexpr KDPoint(KDCoordinate x, KDCoordinate y) : m_x(x), m_y(y) {}
  KDCoordinate x() const { return m_x; }
  KDCoordinate y() const { return m_y; }
  KDPoint translatedBy(KDPoint other) const;
  KDPoint relativeTo(KDPoint p) const { return translatedBy(p.opposite()); }
  KDPoint opposite() const;
  bool operator==(const KDPoint &other) const {
    return (m_x == other.m_x && m_y == other.m_y);
  }
  bool operator!=(const KDPoint &other) const { return !(operator==(other)); }
  uint16_t squareDistanceTo(KDPoint other) const;

 private:
  KDCoordinate m_x;
  KDCoordinate m_y;
};

constexpr KDPoint KDPointZero = KDPoint(0, 0);

#endif
