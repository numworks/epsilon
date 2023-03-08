#ifndef POINCARE_COORDINATE_2D_H
#define POINCARE_COORDINATE_2D_H

#include <math.h>

namespace Poincare {

template <typename T>
class Coordinate2D final {
 public:
  Coordinate2D(T x, T y) : m_x(x), m_y(y) {}
  Coordinate2D() : Coordinate2D(NAN, NAN) {}

  template <class C>
  operator Coordinate2D<C>() {
    return Coordinate2D<C>(static_cast<C>(m_x), static_cast<C>(m_y));
  }

  T x() const { return m_x; }
  T y() const { return m_y; }
  void setX(T x) { m_x = x; }
  void setY(T y) { m_y = y; }

  bool xIsIn(T start, T end, bool includeStart = false,
             bool includeEnd = false) {
    return coordinateIsIn(true, start, end, includeStart, includeEnd);
  }
  bool yIsIn(T start, T end, bool includeStart = false,
             bool includeEnd = false) {
    return coordinateIsIn(false, start, end, includeStart, includeEnd);
  }

 private:
  bool coordinateIsIn(bool x, T start, T end, bool includeStart,
                      bool includeEnd) {
    T var = x ? m_x : m_y;
    return (var > start || (includeStart && var == start)) &&
           (var < end || (includeEnd && var == end));
  }
  T m_x;
  T m_y;
};

}  // namespace Poincare

#endif
