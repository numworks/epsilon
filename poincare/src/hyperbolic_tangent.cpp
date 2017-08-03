#include <poincare/hyperbolic_tangent.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/fraction.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

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

template<typename T>
Complex<T> HyperbolicTangent::compute(const Complex<T> c) {
  if (c.b() == 0) {
    return Complex<T>::Float(std::tanh(c.a()));
  }
  Complex<T> arg1 = HyperbolicSine::compute(c);
  Complex<T> arg2 = HyperbolicCosine::compute(c);
  return Fraction::compute(arg1, arg2);
}

}
