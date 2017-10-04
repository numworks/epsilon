#include <poincare/real_part.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

RealPart::RealPart() :
  Function("re")
{
}

Expression::Type RealPart::type() const {
  return Type::RealPart;
}

Expression * RealPart::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  RealPart * rp = new RealPart();
  rp->setArgument(newOperands, numberOfOperands, cloneOperands);
  return rp;
}

template<typename T>
Complex<T> RealPart::templatedComputeComplex(const Complex<T> c) const {
  return Complex<T>::Float(c.a());
}

}


