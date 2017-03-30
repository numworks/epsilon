#include <poincare/arc_cosine.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

ArcCosine::ArcCosine() :
  Function("acos")
{
}

Expression::Type ArcCosine::type() const {
  return Type::ArcCosine;
}

Expression * ArcCosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ArcCosine * c = new ArcCosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

float ArcCosine::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (angleUnit == AngleUnit::Degree) {
    return acosf(m_args[0]->approximate(context, angleUnit))*180.0f/M_PI;
  }
  return acosf(m_args[0]->approximate(context, angleUnit));
}

}
