#include <poincare/ceiling.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Ceiling::Ceiling() :
  Function("ceil")
{
}

Expression::Type Ceiling::type() const {
  return Type::Ceiling;
}

Expression * Ceiling::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Ceiling * c = new Ceiling();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

Complex Ceiling::computeComplex(const Complex c, AngleUnit angleUnit) const {
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  return Complex::Float(std::ceil(c.a()));
}

}


