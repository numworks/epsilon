#ifndef POINCARE_JUNIOR_NUMERIC_COORDINATE_2D_H
#define POINCARE_JUNIOR_NUMERIC_COORDINATE_2D_H

#include <math.h>
#include <poincare/helpers.h>

namespace PoincareJ {

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

  bool isGreaterThan(const Coordinate2D& other) {
    assert(!std::isnan(m_x) && !std::isnan(m_y));
    assert(!std::isnan(other.m_x) && !std::isnan(other.m_y));
    return (m_x == other.m_x && m_y > other.m_y) || m_x > other.m_x;
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

}  // namespace PoincareJ

#endif
