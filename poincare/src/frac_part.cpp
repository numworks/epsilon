#include <poincare/frac_part.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

FracPart::FracPart() :
  Function("frac")
{
}

Expression::Type FracPart::type() const {
  return Type::FracPart;
}

Expression * FracPart::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  FracPart * fp = new FracPart();
  fp->setArgument(newOperands, numberOfOperands, cloneOperands);
  return fp;
}

template<typename T>
Complex<T> FracPart::templatedComputeComplex(const Complex<T> c) const {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(c.a()-std::floor(c.a()));
}

}


