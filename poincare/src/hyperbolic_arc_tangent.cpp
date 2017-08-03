#include <poincare/hyperbolic_arc_tangent.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

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

Complex HyperbolicArcTangent::computeComplex(const Complex c, AngleUnit angleUnit) const {
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  return Complex::Float(atanhf(c.a()));
}

}
