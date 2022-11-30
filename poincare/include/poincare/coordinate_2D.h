#ifndef POINCARE_COORDINATE_2D_H
#define POINCARE_COORDINATE_2D_H

#include <math.h>

namespace Poincare {

template <typename T>
class Coordinate2D final {
public:
  Coordinate2D(T x1, T x2) : m_x1(x1), m_x2(x2) {}
  Coordinate2D() : Coordinate2D(NAN, NAN) {}

  template<class C>
  operator Coordinate2D<C>() { return Coordinate2D<C>(static_cast<C>(m_x1), static_cast<C>(m_x2)); }

  T x1() const { return m_x1; }
  T x2() const { return m_x2; }
  void setX1(T x1) { m_x1 = x1; }
  void setX2(T x2) { m_x2 = x2; }

  bool x1IsIn(T start, T end, bool includeStart = false, bool includeEnd = false) {
    return coordinateIsIn(true, start, end, includeStart, includeEnd);
  }
  bool x2IsIn(T start, T end, bool includeStart = false, bool includeEnd = false) {
    return coordinateIsIn(false, start, end, includeStart, includeEnd);
  }

private:
  bool coordinateIsIn(bool x1, T start, T end, bool includeStart, bool includeEnd) {
    T var = x1 ? m_x1 : m_x2;
    return (var > start || (includeStart && var == start)) && (var < end || (includeEnd && var == end));
  }
  T m_x1;
  T m_x2;
};

}

#endif
