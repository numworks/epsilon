#include <poincare/tangent.h>
#include <poincare/complex.h>
#include <poincare/sine.h>
#include <poincare/cosine.h>
#include <poincare/fraction.h>
#include <poincare/multiplication.h>
#include <poincare/hyperbolic_tangent.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Tangent::Tangent() :
  TrigonometricFunction("tan")
{
}

Expression * Tangent::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Tangent * t = new Tangent();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

Expression::Type Tangent::type() const {
  return Expression::Type::Tangent;
}

float Tangent::computeForRadianReal(float x) const {
  return tanf(x);
}

Complex Tangent::privateCompute(const Complex c, AngleUnit angleUnit) const {
  Complex result = Fraction::compute(Sine::compute(c), Cosine::compute(c));
  if (!isnan(result.a()) && !isnan(result.b())) {
    return result;
  }
  Complex tanh = HyperbolicTangent::compute(Multiplication::compute(Complex::Cartesian(0.0f, -1.0f), c));
  return Multiplication::compute(Complex::Cartesian(0.0f, 1.0f), tanh);
}

}
