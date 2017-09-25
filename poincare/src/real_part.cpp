#include <poincare/real_part.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type RealPart::type() const {
  return Type::RealPart;
}

Expression * RealPart::clone() const {
  RealPart * a = new RealPart(m_operands, true);
  return a;
}

template<typename T>
Complex<T> RealPart::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.a());
}

}


