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
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  HyperbolicArcSine * s = new HyperbolicArcSine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

float HyperbolicArcSine::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float x = m_args[0]->approximate(context, angleUnit);
  return asinhf(x);
}

}
