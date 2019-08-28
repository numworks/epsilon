#ifndef POINCARE_COORDINATE_2D_H
#define POINCARE_COORDINATE_2D_H

#include <math.h>

namespace Poincare {

template <typename T>
class Coordinate2D final {
public:
  Coordinate2D(T abscissa = NAN, T value = NAN) : m_abscissa(abscissa), m_value(value) {}
  T abscissa() const { return m_abscissa; }
  T value() const { return m_value; }
  void setAbscissa(T a) { m_abscissa = a; }
  void setValue(T v) { m_value = v; }
private:
  T m_abscissa;
  T m_value;
};

}

#endif
