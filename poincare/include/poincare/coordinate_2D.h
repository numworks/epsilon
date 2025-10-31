#pragma once

#include <cmath>

namespace Poincare {

template <typename T>
class Coordinate2D final {
 public:
  using value_type = T;

  Coordinate2D(T x, T y) : m_x(x), m_y(y) {}
  Coordinate2D() : Coordinate2D(NAN, NAN) {}

  template <class C>
  operator Coordinate2D<C>() {
    return Coordinate2D<C>(static_cast<C>(m_x), static_cast<C>(m_y));
  }

  bool operator==(const Coordinate2D& other) const {
    return (x() == other.x() && y() == other.y());
  }

  T x() const { return m_x; }
  T y() const { return m_y; }
  void setX(T x) { m_x = x; }
  void setY(T y) { m_y = y; }

 private:
  T m_x;
  T m_y;
};

}  // namespace Poincare
