#include <poincare/reel_part.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ReelPart::type() const {
  return Type::ReelPart;
}

Expression * ReelPart::clone() const {
  ReelPart * a = new ReelPart(m_operands, true);
  return a;
}

template<typename T>
Complex<T> ReelPart::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.a());
}

}


