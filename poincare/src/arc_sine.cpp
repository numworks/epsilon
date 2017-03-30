#include <poincare/arc_sine.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

ArcSine::ArcSine() :
  Function("asin")
{
}

Expression::Type ArcSine::type() const {
  return Type::ArcSine;
}

Expression * ArcSine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ArcSine * s = new ArcSine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

float ArcSine::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (angleUnit == AngleUnit::Degree) {
    return asinf(m_args[0]->approximate(context, angleUnit))*180.0f/M_PI;
  }
  return asinf(m_args[0]->approximate(context, angleUnit));
}

}
