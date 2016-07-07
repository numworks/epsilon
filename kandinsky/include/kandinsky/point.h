#ifndef KANDINSKY_POINT_H
#define KANDINSKY_POINT_H

#include <kandinsky/coordinate.h>

struct KDPoint {
  constexpr KDPoint(KDCoordinate x, KDCoordinate y) :
    m_x(x), m_y(y) {}
  KDCoordinate x() const;
  KDCoordinate y() const;
  KDPoint translatedBy(KDPoint other) const;
  KDPoint opposite() const;
private:
  KDCoordinate m_x;
  KDCoordinate m_y;
};

constexpr KDPoint KDPointZero = KDPoint(0,0);

#endif
