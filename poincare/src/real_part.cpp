#include <poincare/real_part.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

ReelPart::ReelPart() :
  Function("re")
{
}

Expression::Type ReelPart::type() const {
  return Type::ReelPart;
}

Expression * ReelPart::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ReelPart * rp = new ReelPart();
  rp->setArgument(newOperands, numberOfOperands, cloneOperands);
  return rp;
}

template<typename T>
Complex<T> ReelPart::templatedComputeComplex(const Complex<T> c) const {
  return Complex<T>::Float(c.a());
}

}


