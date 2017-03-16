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
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  HyperbolicArcTangent * t = new HyperbolicArcTangent();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

float HyperbolicArcTangent::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float x = m_args[0]->approximate(context, angleUnit);
  return 0.5f*logf((1.0f+x)/(1.0f-x));
}

}
