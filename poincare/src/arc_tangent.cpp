#include <poincare/arc_tangent.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

ArcTangent::ArcTangent() :
  Function("atan")
{
}

Expression::Type ArcTangent::type() const {
  return Type::ArcTangent;
}

Expression * ArcTangent::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ArcTangent * t = new ArcTangent();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

float ArcTangent::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (angleUnit == AngleUnit::Degree) {
    return atanf(m_args[0]->approximate(context, angleUnit))*180.0f/M_PI;
  }
  return atanf(m_args[0]->approximate(context, angleUnit));
}

}
