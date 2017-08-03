#include <poincare/imaginary_part.h>
#include <poincare/complex.h>

extern "C" {
#include <math.h>
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

Complex ImaginaryPart::computeComplex(const Complex c, AngleUnit angleUnit) const {
  return Complex::Float(c.b());
}

}


