#ifndef POINCARE_COORDINATE_2D_H
#define POINCARE_COORDINATE_2D_H

#include <math.h>

namespace Poincare {

template <typename T>
class Coordinate2D final {
public:
  Coordinate2D(T x1 = NAN, T x2 = NAN) : m_x1(x1), m_x2(x2) {}

  template<class C>
  operator Coordinate2D<C>() { return Coordinate2D<T>(static_cast<C>(m_x1), static_cast<C>(m_x2)); }

  T x1() const { return m_x1; }
  T x2() const { return m_x2; }
  void setX1(T x1) { m_x1 = x1; }
  void setX2(T x2) { m_x2 = x2; }

private:
  T m_x1;
  T m_x2;
};

}

#endif
