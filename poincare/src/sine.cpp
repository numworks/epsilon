#include <poincare/sine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/multiplication.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Sine::Sine() :
  TrigonometricFunction("sin")
{
}

Expression::Type Sine::type() const {
  return Expression::Type::Sine;
}

Expression * Sine::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Sine * s = new Sine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

Complex Sine::compute(const Complex c) {
  Complex arg = Complex::Cartesian(-c.b(), c.a());
  Complex sinh = HyperbolicSine::compute(arg);
  return Multiplication::compute(Complex::Cartesian(0.0f, -1.0f), sinh);
}

float Sine::computeForRadianReal(float x) const {
  return std::sin(x);
}

}
