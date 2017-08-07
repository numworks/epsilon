#include <poincare/cosine.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Cosine::Cosine() :
  TrigonometricFunction("cos")
{
}

Expression::Type Cosine::type() const {
  return Type::Cosine;
}

Expression * Cosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Cosine * c = new Cosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

Complex Cosine::compute(const Complex c) {
  Complex arg = Complex::Cartesian(-c.b(), c.a());
  return HyperbolicCosine::compute(arg);
}

float Cosine::computeForRadianReal(float x) const {
  return std::cos(x);
}

}
