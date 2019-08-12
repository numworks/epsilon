#ifndef POINCARE_COORDINATE_2D_H
#define POINCARE_COORDINATE_2D_H

#include <math.h>

namespace Poincare {

class Coordinate2D final {
public:
  Coordinate2D (double abscissa = NAN, double value = NAN) : m_abscissa(abscissa), m_value(value) {}
  double abscissa() const { return m_abscissa; }
  double value() const { return m_value; }
  void setAbscissa(double a) { m_abscissa = a; }
  void setValue(double v) { m_value = v; }
private:
  double m_abscissa;
  double m_value;
};

}

#endif
