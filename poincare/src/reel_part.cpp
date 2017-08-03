#include <poincare/reel_part.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

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

Complex ReelPart::computeComplex(const Complex c, AngleUnit angleUnit) const {
  return Complex::Float(c.a());
}

}


