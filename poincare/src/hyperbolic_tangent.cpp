#include <poincare/hyperbolic_tangent.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

HyperbolicTangent::HyperbolicTangent() :
  Function("tanh")
{
}

Expression::Type HyperbolicTangent::type() const {
  return Type::HyperbolicTangent;
}

Expression * HyperbolicTangent::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  HyperbolicTangent * ht = new HyperbolicTangent();
  ht->setArgument(newOperands, numberOfOperands, cloneOperands);
  return ht;
}

Complex HyperbolicTangent::compute(const Complex c) {
  if (c.b() == 0.0f) {
    return Complex::Float(tanhf(c.a()));
  }
  Complex arg1 = HyperbolicSine::compute(c);
  Complex arg2 = HyperbolicCosine::compute(c);
  return Fraction::compute(arg1, arg2);
}

}
