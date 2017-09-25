#include <poincare/frac_part.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type FracPart::type() const {
  return Type::FracPart;
}

Expression * FracPart::clone() const {
  FracPart * c = new FracPart(m_operands, true);
  return c;
}

template<typename T>
Complex<T> FracPart::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(c.a()-std::floor(c.a()));
}

}


