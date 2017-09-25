#include <poincare/complex_argument.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ComplexArgument::type() const {
  return Type::ComplexArgument;
}

Expression * ComplexArgument::clone() const {
  ComplexArgument * a = new ComplexArgument(m_operands, true);
  return a;
}

template<typename T>
Complex<T> ComplexArgument::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.th());
}

}

