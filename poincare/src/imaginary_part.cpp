#include <poincare/imaginary_part.h>
#include <poincare/complex.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

namespace Poincare {

Expression::Type ImaginaryPart::type() const {
  return Type::ImaginaryPart;
}

Expression * ImaginaryPart::clone() const {
  ImaginaryPart * a = new ImaginaryPart(m_operands, true);
  return a;
}

template<typename T>
Complex<T> ImaginaryPart::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.b());
}

}


