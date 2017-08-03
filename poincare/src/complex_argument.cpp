#include <poincare/complex_argument.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

ComplexArgument::ComplexArgument() :
  Function("arg")
{
}

Expression::Type ComplexArgument::type() const {
  return Type::ComplexArgument;
}

Expression * ComplexArgument::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ComplexArgument * ca = new ComplexArgument();
  ca->setArgument(newOperands, numberOfOperands, cloneOperands);
  return ca;
}

template<typename T>
Complex<T> ComplexArgument::templatedComputeComplex(const Complex<T> c) const {
  return Complex<T>::Float(c.th());
}

}

