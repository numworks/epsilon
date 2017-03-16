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
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  HyperbolicArcCosine * c = new HyperbolicArcCosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

float HyperbolicArcCosine::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float x = m_args[0]->approximate(context, angleUnit);
  return logf(x+sqrtf(x*x-1.0f));
}

}
