#ifndef POINCARE_COORDINATE_2D_H
#define POINCARE_COORDINATE_2D_H

#include <math.h>

namespace Poincare {

template <typename T>
class Coordinate2D final {
public:
  Coordinate2D(T x = NAN, T y = NAN) : m_x(x), m_y(y) {}
  T x() const { return m_x; }
  T y() const { return m_y; }
  void setX(T x) { m_x = x; }
  void setY(T y) { m_y = y; }
private:
  T m_x;
  T m_y;
};

}

#endif
