#include <poincare/tangent.h>
#include <poincare/complex.h>
#include <poincare/sine.h>
#include <poincare/cosine.h>
#include <poincare/fraction.h>
#include <poincare/multiplication.h>
#include <poincare/hyperbolic_tangent.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Tangent::Tangent() :
  Function("tan")
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

template<typename T>
Complex<T> Tangent::templatedComputeComplex(const Complex<T> c, AngleUnit angleUnit) const {
  Complex<T> result = Fraction::compute(Sine::compute(c, angleUnit), Cosine::compute(c, angleUnit));
  if (!isnan(result.a()) && !isnan(result.b())) {
    return result;
  }
  Complex<T> tanh = HyperbolicTangent::compute(Multiplication::compute(Complex<T>::Cartesian(0, -1), c));
  return Multiplication::compute(Complex<T>::Cartesian(0, 1), tanh);
}

}
