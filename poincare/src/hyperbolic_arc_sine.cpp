#include <poincare/hyperbolic_arc_sine.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

HyperbolicArcSine::HyperbolicArcSine() :
  Function("asinh")
{
}

Expression::Type HyperbolicArcSine::type() const {
  return Type::HyperbolicArcSine;
}

Expression * HyperbolicArcSine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  HyperbolicArcSine * s = new HyperbolicArcSine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

Complex HyperbolicArcSine::computeComplex(const Complex c, AngleUnit angleUnit) const {
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  return Complex::Float(asinhf(c.a()));
}

}
