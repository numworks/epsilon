#include <poincare/hyperbolic_arc_cosine.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

HyperbolicArcCosine::HyperbolicArcCosine() :
  Function("acosh")
{
}

Expression::Type HyperbolicArcCosine::type() const {
  return Type::HyperbolicArcCosine;
}

Expression * HyperbolicArcCosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  HyperbolicArcCosine * c = new HyperbolicArcCosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

Complex HyperbolicArcCosine::computeComplex(const Complex c, AngleUnit angleUnit) const {
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  return Complex::Float(acoshf(c.a()));
}

}
