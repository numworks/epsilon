#include <poincare/floor.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Floor::Floor() :
  Function("floor")
{
}

Expression::Type Floor::type() const {
  return Type::Floor;
}

Expression * Floor::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Floor * f = new Floor();
  f->setArgument(newOperands, numberOfOperands, cloneOperands);
  return f;
}

Complex Floor::computeComplex(const Complex c, AngleUnit angleUnit) const {
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  return Complex::Float(floorf(c.a()));
}

}


