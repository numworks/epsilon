#include <poincare/imaginary_part.h>
#include <poincare/complex.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

namespace Poincare {

ImaginaryPart::ImaginaryPart() :
  Function("im")
{
}

Expression::Type ImaginaryPart::type() const {
  return Type::ImaginaryPart;
}

Expression * ImaginaryPart::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ImaginaryPart * ip = new ImaginaryPart();
  ip->setArgument(newOperands, numberOfOperands, cloneOperands);
  return ip;
}

template<typename T>
Complex<T> ImaginaryPart::templatedComputeComplex(const Complex<T> c) const {
  return Complex<T>::Float(c.b());
}

}


