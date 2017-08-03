#include <poincare/hyperbolic_arc_tangent.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

HyperbolicArcTangent::HyperbolicArcTangent() :
  Function("atanh")
{
}

Expression::Type HyperbolicArcTangent::type() const {
  return Type::HyperbolicArcTangent;
}

Expression * HyperbolicArcTangent::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  HyperbolicArcTangent * t = new HyperbolicArcTangent();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

template<typename T>
Complex<T> HyperbolicArcTangent::templatedComputeComplex(const Complex<T> c) const {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::atanh(c.a()));
}

}
